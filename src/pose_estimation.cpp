#include <iostream>
#include <opencv2/opencv.hpp>
#include "Armor.h"

const float WIDTH = 135/2 ;
const float HEIGHT= 55/2;

using namespace std;    
using namespace cv;

int Armor::pose_estimation()
{
    //灯条上中下六点
    vector<cv::Point3f> objectPoints;
    //灯条上中下六点对应的图像坐标
    vector<cv::Point2f> imagePoints;
    objectPoints.push_back(cv::Point3f(-WIDTH,HEIGHT,0));//1
    objectPoints.push_back(cv::Point3f(-WIDTH,0,0));//2
    objectPoints.push_back(cv::Point3f(-WIDTH,-HEIGHT,0));//3
    objectPoints.push_back(cv::Point3f(WIDTH,HEIGHT,0));//4
    objectPoints.push_back(cv::Point3f(WIDTH,0,0));//5
    objectPoints.push_back(cv::Point3f(WIDTH,-HEIGHT,0));//6
    imagePoints.push_back(cv::Point2f(center.x-WIDTH,center.y-HEIGHT));//1
    imagePoints.push_back(cv::Point2f(center.x-WIDTH,center.y));//2
    imagePoints.push_back(cv::Point2f(center.x-WIDTH,center.y+HEIGHT));//3
    imagePoints.push_back(cv::Point2f(center.x+WIDTH,center.y-HEIGHT));//4
    imagePoints.push_back(cv::Point2f(center.x+WIDTH,center.y));//5
    imagePoints.push_back(cv::Point2f(center.x+WIDTH,center.y+HEIGHT));//6
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) <<  1.7774091341308808e+03, 0., 7.1075979428865026e+02, 0.,
                            1.7754170626354828e+03, 5.3472407285624729e+02, 0., 0., 1.);
    cv:: Mat disCoeffs = (cv::Mat_<double>(1, 5) <<  -5.6313426428564950e-01, 1.8301501710641366e-01,
                          1.9661478907901904e-03, 9.6259122849674621e-04,
                          5.6883803390679100e-01);
    cv::Mat rvec, tvec;
    bool useExtrinsicGuess = false;
    int flags = 0;//基于Levenberg-Marquardt迭代方法计算旋转向量和平移向量
    cv::solvePnP(objectPoints,imagePoints,cameraMatrix,disCoeffs,rvec,tvec,useExtrinsicGuess,flags);
    //计算平移向量的长度
    float distance = cv::norm(tvec, cv::NORM_L2);
    //std::cout<<"distance:"<<distance/10<<std::endl;
    return distance;
}