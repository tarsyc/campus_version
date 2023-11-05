#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include "Buff_manage.h"

using namespace std;
using namespace cv;

Point2f Buff_manage::buff_solve(Mat frame, vector<target> targets)
{
    //使用solvePnP解算扇叶
    //定义世界坐标系下的点
    vector<Point3f> world_points;
    world_points.push_back(Point3f(0,0,0));
    world_points.push_back(Point3f(0,0,0));
    world_points.push_back(Point3f(0,0,0));
    world_points.push_back(Point3f(0,0,0));
    //定义相机坐标系下的点
    vector<Point2f> camera_points;
    camera_points.push_back(Point2f(0,0));
    camera_points.push_back(Point2f(0,0));
    camera_points.push_back(Point2f(0,0));
    
}
