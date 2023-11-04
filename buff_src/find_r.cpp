//使用dft拟合圆
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Buff_manage.h"


using namespace std;
using namespace cv;
Point2f Buff_manage::find_r(Mat frame, Mat frame_process, RotatedRect rect)
{
    // 读取模板图像并转换为HSV颜色空间
    Mat r_template = imread("../config/template_1.png");
    cvtColor(r_template, r_template, COLOR_BGR2HSV);
    inRange(r_template, Scalar(73, 80, 175), Scalar(107, 255, 255), r_template);


    Mat result;
    matchTemplate(frame_process, r_template, result, TM_CCOEFF_NORMED);


    Point maxLoc;
    minMaxLoc(result, nullptr, nullptr, nullptr, &maxLoc);
    

    Point center(maxLoc.x + r_template.cols / 2, maxLoc.y + r_template.rows / 2);
    Rect rect_r(maxLoc.x, maxLoc.y, r_template.cols, r_template.rows);


    circle(frame, center, 5, Scalar(0, 0, 255), 2);

    

    //寻找打击点圆心
    Point2f rect_points[4];
    rect.points(rect_points);
    Point2f center_point;
    float x_min = 10000,x_max = 0,y_min = 10000,y_max = 0;
            for(int j=0;j<4;j++)
            {
                if(rect_points[j].x<x_min)
                x_min = rect_points[j].x;
                if(rect_points[j].x>x_max)
                x_max = rect_points[j].x;
                if(rect_points[j].y<y_min)
                y_min = rect_points[j].y;
                if(rect_points[j].y>y_max)
                y_max = rect_points[j].y;
            }
    Rect tar_rect(static_cast<int>(x_min), static_cast<int>(y_min), static_cast<int>(x_max - x_min), static_cast<int>(y_max - y_min));
    Point2f left_top = Point2f(x_min, y_min);
    Mat find_roi = frame_process.clone();
    //膨胀
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(find_roi, find_roi, element);
    Mat roi_rect = find_roi(tar_rect);
    floodFill(roi_rect, Point2f(0,0), Scalar(255));
    bitwise_not(roi_rect, roi_rect);
    
    imshow("roi_rect", roi_rect);
    vector<vector<Point>> contours;
    findContours(roi_rect, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    for(int i=0;i<contours.size();i++)
    {
        //筛选掉过小的轮廓
        if(contourArea(contours[i])<50)
        {
            contours.erase(contours.begin()+i);
            i--;
        }
    }
    vector<RotatedRect> rotatedRects;
    for(int i=0;i<contours.size();i++)
    {
        rotatedRects.push_back(minAreaRect(contours[i]));
    }
    Point2f tar_point;
    for(int i=0;i<rotatedRects.size();i++)
    {
        float width = rotatedRects[i].size.width;
        float height = rotatedRects[i].size.height;
        double ratio = max(width/height,height/width)/min(width/height,height/width);
        cv::Point2f tarpoint=rotatedRects[i].center + left_top;
        if(ratio<1.5&&ratio>0.5)
        {
            circle(frame, tarpoint, 2, Scalar(0, 0, 255), 2);
            line(frame, center, tarpoint, Scalar(0, 255, 255), 2);//紫色
            tar_point = Point2f(tarpoint.x, tarpoint.y);
            float r = sqrt(pow((center.x - tarpoint.x), 2) + pow((center.y - tarpoint.y), 2));
            circle(frame, center, r, Scalar(255, 0, 255), 2);//紫色
            //将tarpoint和当前timestamp存入vector
            break;
        }
    }
    //返回中心r的外接矩形

    return tar_point;
    
}   
