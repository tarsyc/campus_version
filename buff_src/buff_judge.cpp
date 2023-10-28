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
        //比对角度差和距离差，如果都小于一定值，就认为是同一个buff,更新Buff
        for(int j=0;j<buffs.size();j++)
        {
           
        }
    }
}
