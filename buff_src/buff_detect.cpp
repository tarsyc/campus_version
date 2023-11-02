#include <opencv2/opencv.hpp>
#include <iostream>
#include "Buff_manage.h"
#include <vector>
#include "Buff.h"
#include <openvino/openvino.hpp>
#include <yaml-cpp/yaml.h>

using namespace std;
using namespace cv;
using namespace ov;

Mat Buff_manage::buff_detect(cv::Mat frame)
{
    resize(frame,frame,Size(1920,1200));
    Mat frame_process=frame.clone();
    YAML::Node config = YAML::LoadFile("../config/config.yaml");
    int color = config["color"].as<int>();
    cvtColor(frame_process,frame_process,COLOR_BGR2HSV);
    //***************
    if(color == 1)//blue
    inRange(frame_process, Scalar(73, 80, 175), Scalar(107, 255, 255), frame_process);
    else if(color == 0)//red
    inRange(frame_process, Scalar(0, 100, 92), Scalar(60, 255, 255), frame_process);
    //高斯滤波
    GaussianBlur(frame_process, frame_process, Size(3, 3), 0, 0);
    vector<vector<Point>> contours;//轮廓
    findContours(frame_process, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    for(int i=0;i<contours.size();i++)
    {
        if(contourArea(contours[i])<50)
        {
            contours.erase(contours.begin()+i);
            i--;
        }
    }
    vector<RotatedRect> rotatedRects;
    for(int i=0;i<contours.size();i++)
    {
        RotatedRect rotatedRect = minAreaRect(contours[i]);
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(abs(aspectRatio)<=4)
        rotatedRects.push_back(rotatedRect);
    }
    //画出拟合的椭圆
    for(int i=0;i<rotatedRects.size();i++)
    {
        //ellipse(frame,rotatedRects[i],Scalar(255,255,255),2);
    }
    //尝试合并轮
    vector<vector<Point>> merge_contours;
    vector<double> angles;
    for(int i=0;i<contours.size();i++)
    {
        RotatedRect rotatedRect =minAreaRect(contours[i]);
        double angle = rotatedRect.angle;
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(abs(aspectRatio)>=1)
        {
            bool merged = false;
            for(int j=0;j<merge_contours.size();j++)
            {
                //差值在10度以内，或者差值在-170度以内，或者差值在170度以内
                if(abs(angles[j]-angle)<10.0 || abs(angles[j]-angle+180)<10.0 || abs(angles[j]-angle-180)<10.0)
                {
                    merge_contours[j].insert(merge_contours[j].end(),contours[i].begin(),contours[i].end());
                    angles[j] = (angles[j]+angle)/2.0;
                    merged = true;
                    break;
                }
            }
            if(!merged)
            {
                merge_contours.push_back(contours[i]);
                angles.push_back(angle);
            }
        }
    }
    vector<RotatedRect> final_rects;
    for(int i=0;i<merge_contours.size();i++)
    {
        RotatedRect rotatedRect = minAreaRect(merge_contours[i]);
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(aspectRatio>1 && aspectRatio<5 && rotatedRect.size.area()>500&& rotatedRect.size.area()<10000)
        {
            final_rects.push_back(rotatedRect);
        }
    }
    for(int i=0;i<final_rects.size();i++)
    {
        Point2f vertices[4];
        final_rects[i].points(vertices);
        for(int j=0;j<4;j++)
        {
            line(frame,vertices[j],vertices[(j+1)%4],Scalar(0,255,0),2);
        }
    }
    for(int i=0;i<final_rects.size();i++)
    {
        int lable=buff_cnn(frame_process,final_rects[i]);
        if(lable==0)
        {
            putText(frame,"BUFF_FOUND!",Point2f(100,100),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,255),2);
            //发送停止巡航信号，开始进行精细识别和打击
            break;
        }
    }
    return frame;
}