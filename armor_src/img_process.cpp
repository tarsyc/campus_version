#include <iostream>
#include <opencv2/opencv.hpp>
#include "Light_bar.h"

using namespace std;
using namespace cv;

Mat Light_bar::image_process(Mat frame,int tar_color)
{
    
    //resize(frame, frame, cv::Size(640, 480)); 
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    //根据目标颜色进行二值化
    if(tar_color == 1)//blue
    inRange(hsv, Scalar(73, 20, 175), Scalar(107, 255, 255), hsv);
    else if(tar_color == 0)//red
    inRange(hsv, Scalar(1, 38, 155), Scalar(20, 255, 255), hsv);
    //膨胀
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(hsv, hsv, element);
    return hsv;
}
