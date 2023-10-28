#include <opencv2/opencv.hpp>
#include <iostream>
#include "Buff_manage.h"
#include <vector>
#include "Buff.h"

using namespace std;
using namespace cv;

Mat Buff_manage::buff_recognize(Mat frame, int tar_color)
{
    Mat roi;
    Rect roi_rect(0, 0, 640, 480);//roi范围
    roi = frame(roi_rect);
    cvtColor(roi, roi, COLOR_BGR2GRAY);
    if(tar_color == 1)//blue
    inRange(roi, Scalar(73, 20, 175), Scalar(107, 255, 255), roi);
    else if(tar_color == 0)//red
    inRange(roi, Scalar(0, 100, 92), Scalar(60, 255, 255), roi);
    
    //寻找并合并轮廓
    vector<vector<Point>> contours;
    findContours(roi, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    //首先筛选掉较小的轮廓
    for (int i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) < 100)
        {
            contours.erase(contours.begin() + i);
            i--;
        }
    }
    vector<vector<Point>> merge_contours;
    vector<double>angle;//使用角度进行筛选，将角度相近的轮廓合并
    for (int i = 0; i < contours.size(); i++)
    {
        RotatedRect rect = minAreaRect(contours[i]);
        angle.push_back(rect.angle);
    }
    for (int i = 0; i < contours.size(); i++)
    {
        RotatedRect rect = minAreaRect(contours[i]);
        for (int j = i + 1; j < contours.size(); j++)
        {
            RotatedRect rect1 = minAreaRect(contours[j]);
            if (abs(rect.angle - rect1.angle) < 5)
            {
                contours[i].insert(contours[i].end(), contours[j].begin(), contours[j].end());
                contours.erase(contours.begin() + j);
                j--;
            }
        }
    }
    //筛选掉不符合条件的轮廓
    vector<vector<Point>> final_rects;
    for(int i=0;i<merge_contours.size();i++)
    {
        RotatedRect rect = minAreaRect(merge_contours[i]);
        if (rect.size.width / rect.size.height > 1.2 && rect.size.width / rect.size.height < 2.5 && rect.size.area() > 1000 && rect.size.area() < 10000)
        {
            final_rects.push_back(merge_contours[i]);
        }
    }
    //遍历所有final_rects，将roi旋转到rect的角度，然后截取
    for (int i=0;i<final_rects.size();i++)
    {
        Mat buff_cut;
        Mat rotationMatrix= getRotationMatrix2D(minAreaRect(final_rects[i]).center, minAreaRect(final_rects[i]).angle, 1);
        warpAffine(roi, buff_cut, rotationMatrix, roi.size());
        //截取每个轮廓的图像
        Rect rect = boundingRect(final_rects[i]);
        buff_cut = buff_cut(rect);
        //将buff_cut传入buff_recognize函数，进行识别
    }
    return roi;

}