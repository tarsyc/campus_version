#include <opencv2/opencv.hpp>   
#include <iostream>
#include <vector>
#include "Armor_manage.h"
#include "Armor.h"

using namespace std;    
using namespace cv;

Mat Armor_manager::draw(Mat frame, vector<Armor>& armors) {
    for (int i = 0; i < armors.size(); i++) {
        // 从装甲板对象中获取旋转矩形的参数
        RotatedRect rect;
        rect.angle = armors[i].angle;
        rect.center = armors[i].center;
        rect.size.width = armors[i].width;
        rect.size.height = armors[i].height;

        Point2f rect_points[4];
        rect.points(rect_points); // 计算旋转矩形的四个顶点

        // 绘制旋转矩形
        for (int j = 0; j < 4; j++) {
            line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 1);
        }
        putText(frame, to_string(armors[i].digital), rect_points[3], FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 1);
        putText(frame, to_string(i+1), (rect_points[0]+rect_points[3])/2, FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 255), 1);
    }

    return frame; // 返回处理后的帧
}
