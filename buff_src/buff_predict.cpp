#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include "Buff_manage.h"

using namespace std;
using namespace Eigen;
using namespace cv;

// 角速度函数
double angularSpeedFunction(double t, const Vector3d& params) {
    return params(0) * sin(params(1) * t) + params(2);
}

// 数值积分函数
double integrateAngularSpeed(const Vector3d& params, double t) {
    double dt = 0.001;  // 时间间隔（假设单位是秒）
    double integral = 0.0;
    double current_time = 0.0;
    
    while (current_time < t) {
        double angularSpeed = angularSpeedFunction(current_time, params);
        integral += angularSpeed * dt;
        current_time += dt;
    }
    
    return integral;
}


double Buff_manage::buff_predict(Mat frame, vector<double> omage, vector<double> time) 
{
    // 观测的速度和相应的时间戳
    vector<double> speeds(omage.begin(), omage.end());
    vector<double> timestamps(time.begin(), time.end());
    
    // 初始猜测的参数
    Vector3d initialGuess(0.01,0.01,0.01);

    // 使用非线性最小二乘法拟合模型
    Map<VectorXd> params(initialGuess.data(), initialGuess.size());
    
    for (size_t i = 0; i < timestamps.size(); ++i) {
        double residual = speeds[i] - angularSpeedFunction(timestamps[i], params);
        params += residual * Vector3d(0.01, 0.01, 0.01);  // 使用简单的梯度下降法调整参数
    }

    // 输出拟合后的参数
    //cout << "Fitted Parameters: a=" << params(0) << " omega=" << params(1) << " b=" << params(2) << endl;

    // 预测 t 秒后的角度变化
    double t_prediction = 0.2;  // 预测的时间（假设单位是s）
    double angle_change_prediction = integrateAngularSpeed(params, t_prediction);
    double angle = angle_change_prediction * 180 / M_PI;

    // 输出预测的角度变化
    //cout << "Predicted Angle Change at t=" << t_prediction << " seconds: " << angle << " degrees" << endl;

    return angle;
}
