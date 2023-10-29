#include <opencv2/opencv.hpp>
#include <iostream>
#include "Buff_manage.h"
#include <vector>
#include "Buff.h"

using namespace std;
using namespace cv;

Mat Buff_manage::buff_recognize(Mat frame, int tar_color)
{
    Mat roi,roi_img;
    Rect roi_rect(frame.cols / 2 - 300, frame.rows / 2-300 , 600, 600);//roi范围
    roi = frame(roi_rect);
    roi_img = roi.clone();  
    cvtColor(roi, roi, COLOR_BGR2HSV);
    if(tar_color == 1)//blue
    inRange(roi, Scalar(73, 80, 175), Scalar(107, 255, 255), roi);
    else if(tar_color == 0)//red
    inRange(roi, Scalar(0, 100, 92), Scalar(60, 255, 255), roi);
    
    vector<vector<Point>> contours; 
    findContours(roi, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    const int min_size = 150;
    //对轮廓进行初筛选
    for(int i=0;i<contours.size();i++)
    {
        if(contourArea(contours[i])<min_size)
        {
            contours.erase(contours.begin()+i);
            i--;
        }
    }
    //合并轮廓
    vector<vector<Point>> merge_contours;
    vector<double> angles;
    for(int i=0;i<contours.size();i++)
    {
        RotatedRect rotatedRect =minAreaRect(contours[i]);
        double angle = rotatedRect.angle;
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(abs(aspectRatio)>=1.2)
        {
            bool merged = false;
            for(int j=0;j<merge_contours.size();j++)
            {
                if(abs(angles[j]-angle)<10.0)
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
    vector<RotatedRect> finalRects;
    //对轮廓进行筛选，面积不能太小或太大，并且长宽比不能太大    
    for(int i=0;i<merge_contours.size();i++)
    {
        RotatedRect rotatedRect = minAreaRect(merge_contours[i]);
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(aspectRatio>1.2 && aspectRatio<2.5 && rotatedRect.size.area()>1000 && rotatedRect.size.area()<10000)
        {
            finalRects.push_back(rotatedRect);
        }
    }
    //画出最终的矩形
    for(int i=0;i<finalRects.size();i++)
    {
        Point2f vertices[4];
        finalRects[i].points(vertices);
        for(int j=0;j<4;j++)
        {
            line(roi_img,vertices[j],vertices[(j+1)%4],Scalar(0,255,0),2);
        }
    }
    return roi_img ;
}