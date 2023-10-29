//对识别出来的buff进行筛选，更新他们的位置
#include "Buff_manage.h"
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Buff.h"


using namespace std;
using namespace cv;

void Buff_manage::buff_judge()
{
    //遍历temp_buffs和buffs，对temp_buffs中的buff进行判断，如果是新的buff，就加入buffs中，如果是旧的buff，就更新buffs中的buff
    for(int i=0;i<temp_buffs.size();i++)
    {
        //比对角度差和中心点距离
        for(int j=0;j<buffs.size();j++)
        {
            if(abs(temp_buffs[i].buff_rect.angle-buffs[j].buff_rect.angle)<5&&abs(temp_buffs[i].buff_rect.center.x-buffs[j].buff_rect.center.x)<5&&abs(temp_buffs[i].buff_rect.center.y-buffs[j].buff_rect.center.y)<5)
            {
                buffs[j].update(temp_buffs[i].roi,temp_buffs[i].buff_rect);
                break;
            }
            else if(j==buffs.size()-1)
            {
                buffs.push_back(temp_buffs[i]);
            }
        }
    }
    
}
