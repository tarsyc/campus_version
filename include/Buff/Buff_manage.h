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
    void update(cv::Mat frame)
    {
        cv::Mat roi=buff_recognize(frame,tar_color);//筛选出所有buff，并返回roi进一步处理


    }
    
    private:
    int tar_color = 0;
    cv::Mat buff_recognize(cv::Mat frame,int tar_color);
    void buff_judge();
    void update(cv::Mat frame);
};
std::vector<Buff>temp_buffs;
#endif