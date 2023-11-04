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
    struct target
    {
        cv::Point2f center;
        double timestamp;
    };
    cv::Mat update(cv::Mat frame,double timestamp)
    {
        std::vector<target> targets;
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
                cv::Point2f tar_point=find_r(frame,frame_process,final_rects[i]);
                targets.push_back({tar_point,timestamp});
                break;
            }
        }
        //进行精细识别，准备击打
        if(targets.size()!=0)
        {
            cv::Point2f final_point=buff_solve(frame,targets);
            cv::circle(frame,final_point,5,cv::Scalar(0,0,255),2);
        }
        return frame;
    }
    //***********************
    private:

    int tar_color = 0;
    void buff_detect(cv::Mat frame, cv::Mat frame_peocess,std::vector<cv::RotatedRect> &final_rects);//大符识别函数
    int buff_cnn(cv::Mat frame,cv::Mat frame_process, cv::RotatedRect rect);//判断是否为待击打目标函数
    cv::Point2f find_r(cv::Mat frame,cv::Mat frame_process,cv::RotatedRect rect);//寻找r和待击打点函数
    cv::Point2f buff_solve(cv::Mat frame,std::vector<target> targets);//解算函数
};
#endif