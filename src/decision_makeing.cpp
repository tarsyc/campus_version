#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Armor_manage.h"
#include "Armor.h"
#include "Decision_making.h"

using namespace std;
using namespace cv; 

int Decision_making::decision_making(vector<Armor>& armors)
{
   vector<int>armors_score;
   int max_score=0;
   int max_index=0;
   for(int i=0;i<armors.size();i++)
   {
         //计算装甲板的得分
         //计算装甲板面积
         for(int i=0;i<armors.size();i++)
         {
            armors_score.push_back(0);
            armors_score[i]=armors[i].width*armors[i].height;
         }
         //寻找分数最大装甲板，输出其下标
            
            for(int i=0;i<armors.size();i++)
            {
                max_index=0;
                if(armors_score[i]>max_score)
                {
                    max_score=armors_score[i];
                    max_index=i;
                }
            }
   }
   return max_index;
}