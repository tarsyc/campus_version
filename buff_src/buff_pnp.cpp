#include "Buff_manage.h"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int WEIGHT = 712;
const int HEIGHT = 372;
const double fov = M_PI / 3.0;  // 视场角以弧度表示
const int frame_width = 1920;
const int frame_height = 1080;

double Buff_manage::buff_pnp(Mat frame, RotatedRect rect)
{
    //solvePnP
    float fx = frame_width / (2 * tan(fov / 2));
    float fy = frame_height / (2 * tan(fov / 2));
    float cx = frame_width / 2;
    float cy = frame_height / 2;
    //定义世界坐标系下的点
    vector<Point3f> world_points;
    world_points.push_back(Point3f(0, 0, 0));
    world_points.push_back(Point3f(0, -HEIGHT, 0));
    world_points.push_back(Point3f(WEIGHT, -HEIGHT, 0));
    world_points.push_back(Point3f(WEIGHT, 0, 0));
    //定义相机坐标系下的点
    vector<Point2f> camera_points;
    rect.points(camera_points);
    //定义相机内参
    Mat camera_matrix = (Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    //定义畸变系数
    Mat distortion_coefficients = (Mat_<double>(5, 1) << 0, 0, 0, 0, 0);
    //定义旋转向量和平移向量
    Mat rotation_vector;
    Mat translation_vector;
    //求解PnP
    solvePnP(world_points, camera_points, camera_matrix, distortion_coefficients, rotation_vector, translation_vector);
    //平移向量转换为距离
    double distance = sqrt(translation_vector.dot(translation_vector));
    //cout << "distance:" << distance << endl;
    //旋转向量转换为旋转矩阵
    //通过上一帧和当前帧的旋转矩阵求解角度差值



    return distance;
}


