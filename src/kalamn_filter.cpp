#include "Decision_making.h"
#include "Armor.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

cv::Point2f Decision_making::kalman_filter(cv::Mat& res,Point2f center, float distance)
{
    // 定义和初始化卡尔曼滤波器
    KalmanFilter kf(4, 2, 0); // 状态变量4，测量值2

    // 状态转移矩阵
    kf.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);

    // 测量矩阵
    kf.measurementMatrix = (Mat_<float>(2, 4) << 1, 0, 0, 0, 0, 1, 0, 0);

    // 过程噪声协方差矩阵
    kf.processNoiseCov = (Mat_<float>(4, 4) << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) * 1e-5;

    // 测量噪声协方差矩阵
    kf.measurementNoiseCov = (Mat_<float>(2, 2) << 1, 0, 0, 1) * 1e-1;

    // 初始化卡尔曼滤波的状态和协方差矩阵
    Mat state(4, 1, CV_32F);  // 初始状态
    state.at<float>(0) = center.x;
    state.at<float>(1) = center.y;
    state.at<float>(2) = 0;
    state.at<float>(3) = 0;
    cout<<"["<<center.x<<" "<<center.y<<"]";
    
    Mat state_estimate(4, 1, CV_32F);  // 估计状态
    Mat error_cov(4, 4, CV_32F);       // 估计的协方差矩阵

    // 卡尔曼滤波的时间步长
    float time_step = distance / 25000;
    //cout<<time_step<<endl;
    // 预测
    kf.predict();
    // 更新测量值
    Mat measurement(2, 1, CV_32F);
    measurement.at<float>(0) = center.x;
    measurement.at<float>(1) = center.y;
    // 更新
    kf.correct(measurement);
    // 获取预测的状态
    state_estimate = kf.statePost;
    // 返回预测的中心点
    cv::Point2f predicted_center(state_estimate.at<float>(0), state_estimate.at<float>(1));
    cout<<"["<<predicted_center.x*10000<<" "<<predicted_center.y*10000<<"]"<<endl;
    cv::circle(res, predicted_center*10000, 5, cv::Scalar(255, 0, 0), -1);
    cv::circle(res, center, 5, cv::Scalar(0, 0, 255), 1);
    return predicted_center;
}