#ifndef BUFF_MANAGE_H
#define BUFF_MANAGE_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Buff.h"
#include <vector>
#include <yaml-cpp/yaml.h>
#include <Eigen/Dense>

class Buff_manage
{
public:

    //**************************变量定义
    std::vector<cv::RotatedRect> final_rects;
    cv::Point2f r_center;
    cv::Point2f tarcenter;
    double past_angle;
    double current_angle;
    double past_time;
    double current_time;
    std::vector<double>omega;
    std::vector<double>time;
    //********************************


    cv::Mat update(cv::Mat frame, long long timestamp)
    {
        //************************************图像预处理
        cv::Mat frame_process = frame.clone();
        YAML::Node config = YAML::LoadFile("../config/config.yaml");
        int color = config["color"].as<int>();
        cv::cvtColor(frame_process, frame_process, cv::COLOR_BGR2HSV);
        if (color == 1) // blue
            cv::inRange(frame_process, cv::Scalar(73, 80, 175), cv::Scalar(107, 255, 255), frame_process);
        else if (color == 0) // red
            cv::inRange(frame_process, cv::Scalar(0, 100, 92), cv::Scalar(60, 255, 255), frame_process);
        buff_detect(frame, frame_process, final_rects); // 大符识别
        //std::cout << "final_rects.size():" << final_rects.size() << std::endl;
        //************************************图像预处理


        //***********************************神经网络识别
        //************减少性能占用，首先计算面积最大的那个轮廓
        if(final_rects.empty())//判断是否找到目标
        {
            putText(frame, "BUFF_NOT_FOUND!", cv::Point2f(100, 100), 1, 1, cv::Scalar(0, 0, 255));
        }
        else
        {
        int max_i=0;
        for(int i=0 ;i<final_rects.size();i++)
        {
            if(final_rects[i].size.area()>final_rects[max_i].size.area())
            {
                max_i=i;
            }
        }
        int pred_lable=buff_cnn(frame, frame_process, final_rects[max_i]);
        //std::cout<<pred_lable<<std::endl;
        if (pred_lable == 0)
        {
            putText(frame, "BUFF_FOUND!", cv::Point2f(100, 100), 1, 1, cv::Scalar(0, 0, 255));
            cv::Point2f tar_point = find_r(frame, frame_process, final_rects[max_i]);
            cv::Mat R_now = buff_pnp(frame, final_rects[max_i]);//旋转矩阵
            //使用圆心到待击打点和x轴的夹角作为current_angle
            //使用时间戳作为current_time
            //使用past_angle和past_time作为上一次的角度和时间
            cv::Point2f x_axis(1, 0);
cv::Point2f target_vector = tarcenter - r_center;

double dot_product = x_axis.x * target_vector.x + x_axis.y * target_vector.y;
double magnitude_product = cv::norm(x_axis) * cv::norm(target_vector);

double angle = acos(dot_product / magnitude_product);

// 将角度从弧度转换为度
angle = angle * 180 / CV_PI;

            current_angle=angle;
            current_time=timestamp;
            if(past_angle==0)
            {
                past_angle=current_angle;
            }
            double angle_diff=current_angle-past_angle;
            double time_diff=current_time-past_time;

            double omega1 = angle_diff/time_diff;
            //std::cout<<"angle_diff:"<<angle_diff;
            //std::cout<<"time_diff:"<<time_diff;
            //std::cout<<"omega:"<<omega1<<std::endl;
            if(angle_diff!=0&&time_diff!=0&&angle_diff<10&&angle_diff>-10)
            {
                omega.push_back(omega1);
                time.push_back(time_diff);
            }
            past_angle=current_angle;
            past_time=current_time;
        }
        else
        {
            putText(frame, "BUFF_NOT_FOUND!", cv::Point2f(100, 100), 1, 1, cv::Scalar(0, 0, 255));
        }
        }
        //***********************************完成目标寻找
        //***********************************神经网络识别
        double detal=buff_predict(frame, omega, time);
        std::cout<<"detal:"<<detal<<std::endl;
        //***********************************进行目标预测

        return frame;
    }

private:
    int tar_color = 0;
    void buff_detect(cv::Mat frame, cv::Mat frame_process, std::vector<cv::RotatedRect>& final_rects); // 大符识别函数
    int buff_cnn(cv::Mat frame, cv::Mat frame_process, cv::RotatedRect rect);                             // 判断是否为待击打目标函数
    cv::Point2f find_r(cv::Mat frame, cv::Mat frame_process, cv::RotatedRect rect);                         // 寻找r和待击打点函数
    double buff_predict(cv::Mat frame, std::vector<double> omage, std::vector<double> time);                // 最小二乘法
    cv::Mat buff_pnp(cv::Mat frame, cv::RotatedRect rect);                                                    // solvePnP解算
};

#endif
