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

cv::Point2f calculateyawpitch(double yaw,double pitch,cv::Point2f info, double distance)
{ 
    int fire = 1;
    int frame_width = 1280;
    int frame_height = 720;
    double fov=60;
    float fx = frame_width / (2 * tan(fov / 2));
    float fy = frame_height / (2 * tan(fov / 2));
    float cx = frame_width / 2;
    float cy = frame_height / 2;
    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    //定义畸变系数
    cv::Mat distortion_coefficients = (cv::Mat_<double>(5, 1) << 0, 0, 0, 0, 0);
    //计算归一化坐标
    cv::Point2f norm_point;
    norm_point.x = (info.x - cx) / fx;
    norm_point.y = (info.y - cy) / fy;
    //计算三维坐标
    cv::Mat normalized_point3d=cv::Mat (3,1,CV_64FC1);  
    normalized_point3d.at<double>(0,0)=norm_point.x*distance;
    normalized_point3d.at<double>(1,0)=norm_point.y*distance;
    normalized_point3d.at<double>(2,0)=distance;
    //相机坐标系下的三维坐标转为云台坐标系
    double image_center_x=frame_width/2;
    double image_center_y=frame_height/2;
    double detal_x=info.x-image_center_x;
    double detal_y=info.y-image_center_y;
    //计算焦距
    double f=frame_width/(2*tan(fov/2));
    //计算yaw和pitch
    double yaw1=atan(detal_x/f);
    double pitch1=atan(detal_y/f);
    cv::Point2f yaw_pitch;
    yaw_pitch.x=yaw+yaw1;
    yaw_pitch.y=pitch+pitch1;
    //限制yaw和pitch的范围，yaw0-30，pitch0-360
    if(yaw_pitch.x>30)
    {
        yaw_pitch.x=30;
    }
    else if(yaw_pitch.x<-30)
    {
        yaw_pitch.x=-30;
    }
    if(yaw_pitch.y>360)
    {
        yaw_pitch.y=yaw_pitch.y-360;
    }
    else if(yaw_pitch.y<0)
    {
        yaw_pitch.y=yaw_pitch.y+360;
    }
    return yaw_pitch;
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
    cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
    //加载类
    while (true)
    {
        cv::Mat img;
        std::cout<<"ok";
        //********************************************************************************
        auto message = net.getLatestRecvMessage();
        auto time_now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_system_start).count();
        //*****************************************************************************
        double time_stamp = message.time_stamp;
        img=message.img;
        if(!message.img.empty())
        {
        int fire = 1;
        cv::Mat frame=img.clone();
        if (mode == 1)//识别装甲板
        {
            temp_armors.clear();
            cv::Mat res = light_bar.update(frame, temp_armors, time_stamp); // 灯条识别
            armor_manager.process(res, armors, temp_armors, time_stamp); // 装甲板管理
            armor_manager.draw(res, armors); // 画出装甲板
            int max=decision_making.decision(res, armors); // 决策并跟踪
            //投入calculateyawpitch函数
            //cv::Point2f yaw_pitch = calculateyawpitch(message.yaw,message.pitch,armors[max].center, armors[max].distance);
            cv::Point2f yaw_pitch = cv::Point2f(message.yaw, message.pitch);
            net.sendControlMessage(network::SendStruct(yaw_pitch.x,yaw_pitch.y,fire, 0, 1280, 720));
            cv::imshow("frame", res);
            cv::waitKey(15);
        }
        else if (mode == 0)//识别能量机关
        {
            cv::Point3f target = buff_manage.update(frame, time_stamp);
            //cv::Point2f yaw_pitch = cv::Point2f(target.x, target.y);
            float distance = target.z;
            //cv::Point2f yaw_pitch_info = calculateyawpitch(message.yaw,message.pitch,yaw_pitch, distance);
            //yaw_pitch.x = yaw_pitch_info.x+message.yaw;
            //yaw_pitch.y = yaw_pitch_info.y+message.pitch;
            cv::Point2f yaw_pitch = cv::Point2f(message.yaw, message.pitch);

           net.sendControlMessage(network::SendStruct(yaw_pitch.x,yaw_pitch.y,fire, 0, 1280, 720));

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
