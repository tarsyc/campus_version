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
    void process(cv::Mat& frame,std::vector<Armor>& armors,std::vector<Armor>& temp_armors,double time_stamp)//装甲板管理,更新和增减
    {
        
        armors_select(frame,armors,temp_armors,time_stamp);
        //std::cout<<temp_armors.size()<<std::endl;
        //armors_delect();
    }
    cv::Mat draw(cv::Mat frame,std::vector<Armor>& armors);
    private:
    void armors_select(cv::Mat& frame,std::vector<Armor>& armors,std::vector<Armor>& temp_armors,double time_stamp);//装甲板选择
    
};
#endif  