#include <iostream>
#include <opencv2/opencv.hpp>
#include "Armor_manage.h" 
#include "Armor.h"

using namespace std;
using namespace cv;

void Armor_manager::armors_select(Mat& frame, vector<Armor>& armors, vector<Armor>& temp_armors, long long int timestamp)
{
    const int angleThreshold = 40;
    const int positionThreshold = 150;

    for (const auto& temp_armor : temp_armors)
    {
        bool found = false;
        for (auto& armor : armors)
        {
            if (temp_armor.digital == armor.digital)
            {
                if (abs(temp_armor.angle - armor.angle) < angleThreshold && abs(temp_armor.center.x - armor.center.y) < positionThreshold)
                {
                    armor.update(temp_armor.center, temp_armor.width, temp_armor.height, temp_armor.angle, timestamp);
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            Armor armor(frame, temp_armor.center, temp_armor.width, temp_armor.height, temp_armor.angle, timestamp);
            armors.push_back(armor);
        }
    }

    armors.erase(std::remove_if(armors.begin(), armors.end(), [timestamp](const Armor& armor) {
        return armor.timestamp < timestamp;
    }), armors.end());
}
