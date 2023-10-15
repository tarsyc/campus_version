
#ifndef LIGHT_BAR_H
#define LIGHT_BAR_H
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "Armor.h"
//此类识别图像，创建装甲板
class Light_bar
{
    public:
    Light_bar(int color):tar_color(color){}
    cv::Mat update(cv::Mat frame,std::vector<Armor>& temp_armors,long long int timestamp){
          cv::Mat hsv = image_process(frame,tar_color);
          cv::Mat res = find_contours(frame,hsv,temp_armors,timestamp);
          return res;
    }
    private:
    int tar_color;
    cv::Mat image_process(cv::Mat,int tar_color);
    cv::Mat find_contours(cv::Mat frame,cv::Mat hsv,std::vector<Armor>& temp_armors,long long int timestamp);
};
#endif