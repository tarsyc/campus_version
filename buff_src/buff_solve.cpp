#include "Buff_manage.h"
#include <iostream> 
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Point2f Buff_manage::buff_solve(Mat frame,Mat frame_process,RotatedRect rect)
{
    //计算角速度和角加速度
    float w = 0.0;
    float a = 0.0;
    //计算子弹速度
    //进行距离解算
    const float v= 28.0;
    const float g = 9.8;

    

}