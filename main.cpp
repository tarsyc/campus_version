#include <iostream>
#include <opencv2/opencv.hpp>
#include "Light_bar.h"
#include <yaml-cpp/yaml.h>
#include "Armor_manage.h"
#include "Decision_making.h"
#include "Buff_manage.h"
#include "Buff.h"
#include <chrono>  // 包含时间头文件
#include "NetworkManager.h"
#include <csignal>
bool reg = false;
void sigint_handler(int sig) { exit(1); }

std::vector<Armor> armors;     // 存储所有装甲板
std::vector<Armor> temp_armors; // 存储所有临时装甲板
std::vector<Buff> temp_buffs;   // 存储所有临时buff
std::vector<Buff> buffs;        // 存储所有buff
long long int timestamp = 0;   // 时间戳（以毫秒表示）

cv::Point2f solveyawpitch(float yaw,float pitch,cv::Point2f center)
{
    const int WEIGHT = 712;
    const int HEIGHT = 372;
    const double fov = M_PI / 3.0;  // 视场角以弧度表示
    const int frame_width = 1280;
    const int frame_height = 720;
    float FX = frame_width / (2 * tan(fov / 2));
    float FY = frame_height / (2 * tan(fov / 2));
    float CX = frame_width / 2;
    float CY = frame_height / 2;
    float PI=3.1415926;
    cv::Mat cameraMatrix =(cv::Mat_<double>(3,3)<<FX,0,CX,0,FY,CY,0,0,1);
    cv::Mat distCoeffs = (cv::Mat_<double>(1,5)<<0,0,0,0,0);
    double yaw_gimbal = yaw;  // 从相机坐标系到云台坐标系的Yaw
    double pitch_gimbal = pitch;  // 从相机坐标系到云台坐标系的Pitch
    // 将角度转换为弧度
    double yaw_rad = yaw_gimbal * PI / 180.0;
    double pitch_rad = pitch_gimbal * PI / 180.0;
    // 构建旋转矩阵
    cv::Mat rotationMatrixCameraToGimbal = (cv::Mat_<double>(3,3) <<
        cos(yaw_rad), -sin(yaw_rad), 0,
        sin(yaw_rad), cos(yaw_rad), 0,
        0, 0, 1
    );
    // 注意：根据你的实际情况，可能需要添加关于Pitch的旋转
    // 这里假设Pitch是绕X轴旋转
    cv::Mat rotationMatrixPitch = (cv::Mat_<double>(3,3) <<
        1, 0, 0,
        0, cos(pitch_rad), -sin(pitch_rad),
        0, sin(pitch_rad), cos(pitch_rad)
    );
    // 将Pitch的旋转矩阵应用到总的旋转矩阵上
    rotationMatrixCameraToGimbal = rotationMatrixPitch * rotationMatrixCameraToGimbal;
        // 将目标像素点转换为相机坐标系下的坐标
    cv::Mat target_pixel_point(3, 1, CV_64F);
    target_pixel_point.at<double>(0, 0) = center.x;
    target_pixel_point.at<double>(1, 0) = center.y;
    target_pixel_point.at<double>(2, 0) = 1.0;
    // 使用相机内外参数和畸变系数进行坐标转换
    cv::Mat target_camera_point;
    target_camera_point = cameraMatrix.inv() * target_pixel_point;
    // 转换相机坐标系到云台坐标系
    cv::Mat target_gimbal_point =   rotationMatrixCameraToGimbal * target_camera_point;
    // 计算Yaw和Pitch
    /* newYaw = atan2(target_gimbal_point.at<double>(0, 0), target_gimbal_point.at<double>(2, 0)) * 180.0 / PI;
    newPitch = atan2(target_gimbal_point.at<double>(1, 0), target_gimbal_point.at<double>(2, 0)) * 180.0 / PI;
    */
    double newyaw,newpitch;
    newyaw=yaw+atan2(target_camera_point.at<double>(0, 0), target_camera_point.at<double>(2, 0)) * 180.0 / PI;
    newpitch=pitch-atan2(target_camera_point.at<double>(1, 0), target_camera_point.at<double>(2, 0)) * 180.0 / PI;
    return cv::Point2f(newyaw,newpitch);
}

int main()
{
    //******************************************
    signal(SIGINT, sigint_handler);
    // 创建 NetworkManager 对象
    network::NetworkManager net("127.0.0.1", 20205654, "梗小姐赢麻了", 5558,5559);
    // 注册视觉程序
    reg = net.registerUser(0, 100000);
    if (!reg) {
      std::cout << "Register Failed, Exit" << std::endl;
      exit(1);
    }
    
    std::cout << "Register Success! the client will start at 2 second" << std::endl;
    sleep(2);
    auto time_system_start = std::chrono::system_clock::now();
    auto time_temp = std::chrono::system_clock::now();
    //******************************************
    YAML::Node config = YAML::LoadFile("../config/config.yaml");
    int color = config["color"].as<int>();
    Light_bar light_bar(color); // 加载灯条识别类
    Armor_manager armor_manager; // 加载装甲板管理类
    Decision_making decision_making; // 加载决策类
    Buff_manage buff_manage; // 加载buff管理类
    int mode = 1;
    //cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
    auto message = net.getLatestRecvMessage();
    //***********************************
     std::vector<cv::Point> points = {
        {134, 15},    // 示例点1的yaw和pitch
        {140, 18},    // 示例点2的yaw和pitch
        {145,9},    // 示例点3的yaw和pitch
        {140, 6}     // 示例点4的yaw和pitch
    };
    double centerX =  0.0;
    double centerY = 0.0;
    double radius = 6.26997;
    double angle=1.0;
    bool is=false;
    //********************************************
    //加载类
    while (true)
    {
        cv::Mat img;
        //********************************************************************************
        
        auto time_now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_system_start).count();
        //*****************************************************************************
        double time_stamp = message.time_stamp;
        img=message.img;
        if(!message.img.empty())
        {
        int fire = 1;
        cv::Mat frame=img.clone();
        if (mode==1)//识别装甲板
        {
            temp_armors.clear();
            cv::Mat res = light_bar.update(frame, temp_armors, time_stamp); // 灯条识别
            armor_manager.process(res, armors, temp_armors, time_stamp); // 装甲板管理
            armor_manager.draw(res, armors); // 画出装甲板
            int max=decision_making.decision(res, armors);// 决策并跟踪
            //cv::Point2f yaw_pitch = calculateyawpitch(message.yaw,message.pitch,armors[max].center, armors[max].distance);
            cv::Point2f yaw_pitch = cv::Point2f(message.yaw, message.pitch);
            net.sendControlMessage(network::SendStruct(yaw_pitch.x,yaw_pitch.y,fire, 0, 1280, 720));

            //cv::imshow("frame", res);
            //cv::waitKey(150);
        }
        else if (mode==0)//识别能量机关
        {
            cv::Point3f target = buff_manage.update(frame, time_stamp);
            //cv::Point2f yaw_pitch = cv::Point2f(target.x, target.y);
            //float distance = target.z;
            //cv::Point2f yaw_pitch_info = solveyawpitch(message.yaw,message.pitch,yaw_pitch);
            //yaw_pitch.x = yaw_pitch_info.x+message.yaw;
            //yaw_pitch.y = yaw_pitch_info.y+message.pitch;
            //cv::Point2f yaw_pitch = cv::Point2f(message.yaw, message.pitch);
            //net.sendControlMessage(network::SendStruct(yaw_pitch_info.x,yaw_pitch_info.y,fire, 0, 1280, 720));
            cv::Point2f nowyawpitch = cv::Point2f(message.yaw, message.pitch);
            std::cout<<"yaw "<<message.yaw<<std::endl;
            std::cout<<"pitch"<<message.pitch<<std::endl;
            //合入这里
            //按下f记录当前yaw和pitch
           
                fire=1;
                const double rotationSpeed =0.1;
                double x=radius*cos(angle)+centerX;
                double y=radius*sin(angle)+centerY;
                net.sendControlMessage(network::SendStruct(x,y,fire, 0, 1280, 720));
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                angle+=rotationSpeed;
        }
      float  rotaion_speed = 1;
      auto time_duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_now-time_temp);
      time_temp=time_now;
      int shoot_rate=1;
      std::cout<<"yaw "<<message.yaw<<std::endl;
      std::cout<<"pitch"<<message.pitch<<std::endl;
      }
      else
      {
        std::cout << "Get an empty image~" << std::endl;
      }
    }
}
