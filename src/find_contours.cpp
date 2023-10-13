#include <iostream>
#include <opencv2/opencv.hpp>
#include "Armor.h"
#include "Light_bar.h"  
#include <vector>
using namespace std;
using namespace cv;

Mat Light_bar::find_contours(Mat frame, Mat hsv, vector<Armor>& temp_armors,long long int timestamp)
{
    // 寻找轮廓，使用椭圆拟合轮廓
    resize(frame, frame, cv::Size(640, 480));
    vector<vector<Point>> contours;
    vector<cv::RotatedRect>lightbar;
    lightbar.clear();
    findContours(hsv, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // 创建一个与轮廓数量相等的 box 向量
    vector<RotatedRect> box(contours.size());

    // 筛选轮廓，轮廓要大于100，且长宽比在一定范围内
    for (int i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) > 75)
        {
            RotatedRect minRect = minAreaRect(contours[i]);
            Point2f rect_points[4];
            minRect.points(rect_points);
            float judge = minRect.size.width / minRect.size.height;
            if(minRect.angle>60&&judge>=2){
            //合格装甲板存入lightbar
            lightbar.push_back(minRect);
            for (int j = 0; j < 4; j++)
            {
            line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 2);
            }
          }
        }
    }
    //检测灯条组成装甲板
    for(int i=0;i<lightbar.size();i++)
    {
        for(int j=i+1;j<lightbar.size();j++)
        {
            if(abs(lightbar[i].angle-lightbar[j].angle)<5)
            {
                //计算装甲板的旋转矩形
                cv::Point2f center = (lightbar[i].center+lightbar[j].center)/2;//中心点
                float angle=(lightbar[i].angle+lightbar[j].angle)/2;//角度
                float width=abs(lightbar[i].center.x-lightbar[j].center.x);
                float height=abs(lightbar[i].center.x-lightbar[j].center.x);
                //将临时装甲板存储到Armor_manager中,等待进一步处理
                Armor temp_armor(frame,center,width,height,angle,timestamp);
                temp_armors.push_back(temp_armor);
            }
        }
    }
    return frame;
}
