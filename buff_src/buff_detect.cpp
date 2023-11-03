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

void Buff_manage::buff_detect(cv::Mat frame, cv::Mat frame_process,std::vector<RotatedRect> &final_rects)
{

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
    vector<double> angles;
    vector<vector<Point>>contours2;
    for(int i=0;i<contours.size();i++)
    {
        RotatedRect rotatedRect = minAreaRect(contours[i]);
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(abs(aspectRatio)<=3&&abs(aspectRatio)>=1.2&&rotatedRect.size.area()>300&&rotatedRect.size.area()<10000){
        rotatedRects.push_back(rotatedRect);
        angles.push_back(rotatedRect.angle);
        contours2.push_back(contours[i]);
        }
    }
    //画出拟合的旋转矩形
    for(int i=0;i<rotatedRects.size();i++)
    {
        Point2f vertices[4];
        rotatedRects[i].points(vertices);
        for (int j = 0; j < 4; j++)
        //line(frame, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 2);
        putText(frame, to_string(rotatedRects[i].angle), rotatedRects[i].center, 1, 1, Scalar(0, 0, 255));
    }
    //**********************************************
    //合并角度相近的轮廓
    vector<vector<Point>>merge_contours;
    for(int i=0;i<contours2.size();i++)
    {
        bool merged = false;
        for(int j=0;j<merge_contours.size();j++)
        {
            if(abs(angles[j]-angles[i])<15.0)
            {
                merge_contours[j].insert(merge_contours[j].end(),contours2[i].begin(),contours2[i].end());
                merged = true;
                break;
            }
        }
        if(!merged)
        {
            merge_contours.push_back(contours2[i]);
        }
    }
    //使用旋转矩形拟合轮廓
    for(int i=0;i<merge_contours.size();i++)
    {
        RotatedRect rotatedRect = minAreaRect(merge_contours[i]);
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(rotatedRect.size.area()>200 && rotatedRect.size.area()<10000)
        {
            final_rects.push_back(rotatedRect);
        }
    }
    //画出最终的矩形
    for(int i=0;i<final_rects.size();i++)
    {
        Point2f vertices[4];
        final_rects[i].points(vertices);
        for(int j=0;j<4;j++)
        {
            line(frame,vertices[j],vertices[(j+1)%4],Scalar(0,255,0),2);
        }
    }
}
