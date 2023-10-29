#ifndef BUFF_H
#define BUFF_H

#include <opencv2/opencv.hpp>
#include <iostream>


class Buff//Buff基类,每个buff都继承这个类，自识别，自跟踪
{
    public:
    bool is_buff0 = false;
    cv::RotatedRect buff_rect;
    Buff(cv::Mat& roi,cv::RotatedRect buff_rect):buff_rect(buff_rect)
    {
        //bool is_buff0= buff_recognize(roi);
        //track_point = buff_rect.center;
    }
    void update(cv::Mat& roi,cv::RotatedRect buff_rect)
    {
        //bool is_buff0= buff_recognize(roi);
       // track_point = buff_rect.center;
    }
    private:
    //int buff_recognize(cv::Mat roi);
    //cv::Point2f track_point;
};
#endif