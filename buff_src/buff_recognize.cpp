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
    const int min_size = 100;
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
    for(int i=0;i<contours.size();i++)
    {   
        bool found = false;
        RotatedRect rect1 = minAreaRect(contours[i]);
        for(int j=i+1;j<contours.size();j++)
        {
            RotatedRect rect2 = minAreaRect(contours[j]);
            if(abs(rect1.angle-rect2.angle)<5&&abs(rect1.center.x-rect2.center.x)<5&&abs(rect1.center.y-rect2.center.y)<5)
            {
                merge_contours.push_back(contours[i]);
                merge_contours.push_back(contours[j]);
            }
        }
        if(!found)
        {
            merge_contours.push_back(contours[i]);
        }
    }
    //对合并后的轮廓进行筛选
    vector<RotatedRect> final_rects;
    for(int i=0;i<merge_contours.size();i++)
    {
        RotatedRect rect = minAreaRect(merge_contours[i]);
        if(rect.size.width*rect.size.height>100&&rect.size.width*rect.size.height<8000)
        {
            final_rects.push_back(rect);
        }
    }

    cout<<"final_rects.size():"<<final_rects.size()<<endl;
    //绘制出所有轮廓
    for(int i=0;i<final_rects.size();i++)
    {
        Point2f vertices[4];
        final_rects[i].points(vertices);
        for (int j = 0; j < 4; j++)
        {
            line(roi_img, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 2);
        }
    }
    return roi_img ;
}