#include <iostream>
#include <opencv2/opencv.hpp>
#include "Light_bar.h"
#include <yaml-cpp/yaml.h>
#include "Armor_manage.h"
#include "Decision_making.h"


std::vector<Armor> armors;//存储所有装甲板
std::vector<Armor> temp_armors;//存储所有临时装甲板
long long int timestamp=0;

int main()
{
    YAML::Node config = YAML::LoadFile("../config/config.yaml");
    int color = config["color"].as<int>();
    Light_bar light_bar(color);//加载灯条识别类
    Armor_manager armor_manager;//加载装甲板管理类
    Decision_making decision_making;//加载决策类
    cv::VideoCapture cap("/home/tarsyc/下载/T-DT2023-OpenCV/video/Infantry_blue.avi");
    cv::Mat frame;
    cv::namedWindow("frame",cv::WINDOW_AUTOSIZE);
    while(true)
    {
        cap >> frame;
        temp_armors.clear();
        cv::Mat res=light_bar.update(frame,temp_armors,timestamp);//灯条识别
        armor_manager.process(res,armors,temp_armors,timestamp);//装甲板管理
        armor_manager.draw(res,armors);//画出装甲板
        decision_making.decision(res,armors);//决策并跟踪
        cv::imshow("frame",res);;
        timestamp++;
        //cv::waitKey(0);
        if(cv::waitKey(30) == 'q')                                   
            break;
    }   
} 