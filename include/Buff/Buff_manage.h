#ifndef BUFF_MANAGE_H    
#define BUFF_MANAGE_H
#include <iostream>
#include "vector"
#include "opencv2/opencv.hpp"
#include "Buff.h"

class Buff_manage
{
    public:
    //对大符进行识别，处理，跟踪
    //构造函数
    cv::Mat buff_recognize(cv::Mat frame,int tar_color);
    private:
    int tar_color = 0;
    //void buff_judge();
};
#endif