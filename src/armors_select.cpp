#include <iostream>
#include <opencv2/opencv.hpp>
#include "Armor_manage.h" 
#include "Armor.h"

using namespace std;
using namespace cv;

void Armor_manager::armors_select(Mat& frame,vector<Armor>& armors,vector<Armor>& temp_armors,long long int timestamp)
{
    for(int i=0;i<temp_armors.size();i++)
    {
        bool found=false;
        for(int j=0;j<armors.size();j++)
        {  
            if(temp_armors[i].digital=armors[j].digital)
            {
                if(abs(temp_armors[i].angle-armors[j].angle)<40&&abs(temp_armors[i].center.x-armors[j].center.y)<150)//
                {
                    armors[j].update(temp_armors[i].center,temp_armors[i].width,temp_armors[i].height,temp_armors[i].angle,timestamp);//更新装甲板数据
                    found=true;
                }
            }
        }
        if(found==false)
        {
            Armor armor(frame,temp_armors[i].center,temp_armors[i].width,temp_armors[i].height,temp_armors[i].angle,timestamp);
            armors.push_back(armor);//创建新的装甲板

        }
    }
    for(int i=0;i<armors.size();i++)
    {
        if(armors[i].timestamp<timestamp)
        {
            armors.erase(armors.begin()+i);//删除不合格的装甲板
        }
    }
}