#include <iostream>
#include <opencv2/opencv.hpp>
#include "Light_bar.h"
#include <yaml-cpp/yaml.h>
#include "Armor_manage.h"
#include "Decision_making.h"
#include "Buff_manage.h"
#include "Buff.h"
#include <chrono>  // 包含时间头文件

std::vector<Armor> armors;     // 存储所有装甲板
std::vector<Armor> temp_armors; // 存储所有临时装甲板
std::vector<Buff> temp_buffs;   // 存储所有临时buff
std::vector<Buff> buffs;        // 存储所有buff
long long int timestamp = 0;   // 时间戳（以毫秒表示）

int main()
{
    YAML::Node config = YAML::LoadFile("../config/config.yaml");
    int color = config["color"].as<int>();
    Light_bar light_bar(color); // 加载灯条识别类
    Armor_manager armor_manager; // 加载装甲板管理类
    Decision_making decision_making; // 加载决策类
    Buff_manage buff_manage; // 加载buff管理类
    cv::VideoCapture cap("/home/tarsyc/energy.mp4");
    cv::Mat frame;
    int mode = 0;
    cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);

    while (true)
    {
        cap >> frame;

        // 获取当前系统时间戳（以毫秒为单位）
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        if (mode == 1)
        {
            temp_armors.clear();
            cv::Mat res = light_bar.update(frame, temp_armors, timestamp); // 灯条识别
            armor_manager.process(res, armors, temp_armors, timestamp); // 装甲板管理
            armor_manager.draw(res, armors); // 画出装甲板
            decision_making.decision(res, armors); // 决策并跟踪
            cv::imshow("frame", res);
        }
        else if (mode == 0)
        {
            cv::Mat res = buff_manage.update(frame, timestamp);
            cv::imshow("frame", res);
        }

        if (cv::waitKey(1) == 'q')
            break;
    }
}
