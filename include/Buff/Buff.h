#ifndef BUFF_H
#define BUFF_H

#include <opencv2/opencv.hpp>
#include <iostream>


class Buff//Buff基类,每个buff都继承这个类，自检测，自识别，自跟踪
{
    public:
    bool is_buff0 = false;
    cv::RotatedRect buff_rect;

    private:
    int buff_recognize(cv::Mat roi);
    cv::Point2f track_point;
};

std::vector<Buff>buffs;//存储所有buff

#endif