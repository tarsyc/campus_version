#ifndef DECISION_MAKING_H
#define DECISION_MAKING_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Armor.h"

class Decision_making 
{
    public:
    void decision(cv::Mat& res,std::vector<Armor>& armors)
    {
        int i=decision_making(armors);
        kalman_filter(res,armors[i].center,armors[i].distance);
    }
    private:
    int decision_making(std::vector<Armor>& armors);
    cv::Point2f kalman_filter(cv::Mat& res,cv::Point2f center, float distance);
};
#endif