#ifndef BUFF_MANAGE_H    
#define BUFF_MANAGE_H
#include <iostream>
#include "vector"
#include "opencv2/opencv.hpp"
#include "Buff.h"
#include <vector>
#include <yaml-cpp/yaml.h>

class Buff_manage
{
    public:
    //***********************
    cv::Mat update(cv::Mat frame)//更新函数
    {

        std::vector<cv::RotatedRect> final_rects;
        cv::Mat frame_process=frame.clone();
        YAML::Node config = YAML::LoadFile("../config/config.yaml");
        int color = config["color"].as<int>();
        cv::cvtColor(frame_process,frame_process,cv::COLOR_BGR2HSV);
        if(color == 1)//blue
        cv::inRange(frame_process, cv::Scalar(73, 80, 175), cv::Scalar(107, 255, 255), frame_process);
        else if(color == 0)//red
        cv::inRange(frame_process, cv::Scalar(0, 100, 92), cv::Scalar(60, 255, 255), frame_process);
        buff_detect(frame,frame_process,final_rects);//大符识别
        //神经网络识别
        for(int i=0;i<final_rects.size();i++)
        {
            int pred=buff_cnn(frame,frame_process,final_rects[i]);
            if(pred==0)
            {
                putText(frame,"BUFF_FOUND!",cv::Point2f(100,100),1,1,cv::Scalar(0,0,255));
                find_r(frame,frame_process,final_rects[i]);   
                break;
            }
        }
        //进行精细识别，准备击打

        return frame;
    }
    //***********************
    private:
    int tar_color = 0;
    void buff_detect(cv::Mat frame, cv::Mat frame_peocess,std::vector<cv::RotatedRect> &final_rects);//大符识别函数
    int buff_cnn(cv::Mat frame,cv::Mat frame_process, cv::RotatedRect rect);//判断是否为待击打目标函数
    void find_r(cv::Mat frame,cv::Mat frame_process,cv::RotatedRect rect);//寻找r通道函数
};
#endif