#ifndef DECISION_MAKING_H
#define DECISION_MAKING_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Armor.h"

class Decision_making 
{
    public:
    int decision(cv::Mat& res, std::vector<Armor>& armors) {
    if (armors.empty()) {
        // 处理 armors 为空的情况
        return -1;
    }
    i = decision_making(armors);
    if (i >= 0 && i < armors.size()) {
        kalman_filter(res, armors[i].center, armors[i].distance);
    }
    return i;
}

    private:
    int i;
    int decision_making(std::vector<Armor>& armors);
    cv::Point2f kalman_filter(cv::Mat& res,cv::Point2f center, float distance);
};
#endif