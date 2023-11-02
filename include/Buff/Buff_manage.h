#ifndef BUFF_MANAGE_H    
#define BUFF_MANAGE_H
#include <iostream>
#include "vector"
#include "opencv2/opencv.hpp"
#include "Buff.h"
#include <vector>

class Buff_manage
{
    public:
    //***********************
    cv::Mat update(cv::Mat frame)//更新函数
    {

        cv::Mat res=buff_detect(frame);
        return res;
    }
    //***********************
    private:
    int tar_color = 0;
    cv::Mat buff_detect(cv::Mat frame);//大符识别函数
    int buff_cnn(cv::Mat frame, cv::RotatedRect rect);//判断是否为待击打目标函数
};
#endif