#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include "Buff_manage.h"

using namespace std;
using namespace cv;

Point2f Buff_manage::buff_solve(Mat frame, vector<target> targets)
{
    // Extract data from targets
    double t = 0.1;
    vector<double> timestamps;
    vector<Point2f> centers;

    for (const auto& target : targets) {
        timestamps.push_back(target.timestamp);
        centers.push_back(target.center);
    }

    // Convert timestamps to time delta (seconds)
    for (double& timestamp : timestamps) {
        timestamp -= targets.front().timestamp;
    }

    // Polynomial regression (2nd degree, you can change the degree as needed)
    int degree = 2;
    Eigen::MatrixXd A(timestamps.size(), degree + 1);
    Eigen::VectorXd b(centers.size());

    for (int i = 0; i < timestamps.size(); i++) {
        A(i, 0) = 1.0; // Constant term
        for (int j = 1; j <= degree; j++) {
            A(i, j) = std::pow(timestamps[i], j);
        }
        b(i) = centers[i].x; // X-coordinate, adjust for Y-coordinate as needed
    }

    Eigen::VectorXd coefficients = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
    Eigen::VectorXd coefficients2 = A.colPivHouseholderQr().solve(b);

    // Predict future position at time t
    double x_prediction = 0;
    double y_prediction = 0;
    for (int j = 0; j <= degree; j++) {
        x_prediction += coefficients(j) * std::pow(t, j);
        y_prediction += coefficients2(j) * std::pow(t, j);
    }

    // Similar steps for Y-coordinate if needed

    Point2f predicted_position(x_prediction, y_prediction); // Adjust Y-coordinate if needed

    return predicted_position;
}
