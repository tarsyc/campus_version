#ifndef ARMOR_H
#define ARMOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>   
//装甲板基类
class Armor
{
    public:
    float angle;
    cv::Point2f center;
    int digital;
    float width;
    float height;
    float distance;
    long long int timestamp;
    std::vector<cv::Point2f>objectPoints;
    std::vector<cv::Point2f>imagePoints;
    Armor(cv::Mat& frame,const cv::Point2f& center, float width, float height, float angle,long long int timestamp) : center(center), width(width), height(height), angle(angle),timestamp(timestamp) {
    //digital = cnn_recognize(frame);
    distance = pose_estimation();
    }
    void update(cv::Point2f center, float width, float height, float angle, long long int timestamp)//更新函数
    {
        this->center=center;
        this->width=width;
        this->height=height;
        this->angle=angle;
        this->timestamp=timestamp;
    }
    private:
    int pose_estimation();
    int cnn_recognize(cv::Mat& frame);
};
#endif