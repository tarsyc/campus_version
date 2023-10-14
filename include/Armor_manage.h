#ifndef ARMOR_MANAGE_H
#define ARMOR_MANAGE_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Armor.h"
#include "Light_bar.h"
#include <vector>


class Armor_manager
{
    public:
    //对临时装甲板进行筛选
    //若距离小于一定值，且角度小于一定值，则认为是同一个装甲板
    //更新装甲板数据
    //若距离大于一定值，则认为是不同的装甲板
    //创建新的装甲板
    //清空临时装甲板
    //构造函数
    void process(cv::Mat& frame,std::vector<Armor>& armors,std::vector<Armor>& temp_armors,long long int timestamp)//装甲板管理,更新和增减
    {
        armors_select(frame,armors,temp_armors,timestamp);
        std::cout<<temp_armors.size()<<std::endl;
        //armors_delect();
    }
    cv::Mat draw(cv::Mat frame,std::vector<Armor>& armors);
    private:
    void armors_select(cv::Mat& frame,std::vector<Armor>& armors,std::vector<Armor>& temp_armors,long long int timestamp);
    //void armors_delect();
};
#endif  