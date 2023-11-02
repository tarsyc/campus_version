#include <opencv2/opencv.hpp>
#include <iostream>
#include "Buff_manage.h"
#include <vector>
#include "Buff.h"
#include <openvino/openvino.hpp>

using namespace std;
using namespace cv;
using namespace ov;

Mat buff_recognize(Mat frame, int tar_color)
{
    Mat roi,roi_img;
    Rect roi_rect(frame.cols / 2 - 300, frame.rows / 2-300 , 600, 600);//roi范围
    roi = frame(roi_rect);
    roi_img = roi.clone();  
    cvtColor(roi, roi, COLOR_BGR2HSV);
    if(tar_color == 1)//blue
    inRange(roi, Scalar(73, 80, 175), Scalar(107, 255, 255), roi);
    else if(tar_color == 0)//red
    inRange(roi, Scalar(0, 100, 92), Scalar(60, 255, 255), roi);
    
    vector<vector<Point>> contours; 
    findContours(roi, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    const int min_size = 150;
    //对轮廓进行初筛选
    for(int i=0;i<contours.size();i++)
    {
        if(contourArea(contours[i])<min_size)
        {
            contours.erase(contours.begin()+i);
            i--;
        }
        //寻找实心圆形轮廓

    }
    vector<vector<Point>> merge_contours;
    vector<double> angles;
    for(int i=0;i<contours.size();i++)
    {
        RotatedRect rotatedRect =minAreaRect(contours[i]);
        double angle = rotatedRect.angle;
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(abs(aspectRatio)>=1.2)
        {
            bool merged = false;
            for(int j=0;j<merge_contours.size();j++)
            {
                if(abs(angles[j]-angle)<10.0)
                {
                    merge_contours[j].insert(merge_contours[j].end(),contours[i].begin(),contours[i].end());
                    angles[j] = (angles[j]+angle)/2.0;
                    merged = true;
                    break;
                }
            }
            if(!merged)
            {
                merge_contours.push_back(contours[i]);
                angles.push_back(angle);
            }
        }
    }
    vector<RotatedRect> finalRects;
    //对轮廓进行筛选，面积不能太小或太大，并且长宽比不能太大,并且寻找1:1的实心圆形轮廓

    for(int i=0;i<merge_contours.size();i++)
    {
        RotatedRect rotatedRect = minAreaRect(merge_contours[i]);
        double aspectRatio = std::max(rotatedRect.size.width, rotatedRect.size.height) / std::min(rotatedRect.size.width, rotatedRect.size.height);
        if(aspectRatio>1.2 && aspectRatio<2.5 && rotatedRect.size.area()>1000 && rotatedRect.size.area()<10000)
        {
            finalRects.push_back(rotatedRect);
        }
    }
    //画出最终的矩形
    for(int i=0;i<finalRects.size();i++)
    {
        Point2f vertices[4];
        finalRects[i].points(vertices);
        for(int j=0;j<4;j++)
        {
            line(roi_img,vertices[j],vertices[(j+1)%4],Scalar(0,255,0),2);
        }
    }
    //******************************************************************
    //将roi_img 旋转到旋转矩形的角度，然后框选
    vector<Mat> roi_cuts;
    for(int i=0;i<finalRects.size();i++)
    {
        Mat roi_cut;
        Mat rotationMatrix=getRotationMatrix2D(finalRects[i].center,finalRects[i].angle,1);
        warpAffine(roi_img,roi_cut,rotationMatrix,roi_img.size());
        //截取区域并存入vector
        Rect rect(finalRects[i].center.x-finalRects[i].size.width/2,finalRects[i].center.y-finalRects[i].size.height/2,finalRects[i].size.width,finalRects[i].size.height);
        roi_cut = roi_cut(rect);
        roi_cuts.push_back(roi_cut);
    }
    //使用openvino进行识别
    Core core;
    CompiledModel model = core.compile_model("../config/nn.onnx", "CPU");
    InferRequest infer_request = model.create_infer_request();
    Tensor input_tensor = infer_request.get_input_tensor(0);
    Shape tensor_shape = input_tensor.get_shape();
    for (int i = 0; i < roi_cuts.size(); i++) {
        // 图像预处理
        Mat roi_image = roi_cuts[i];
        cvtColor(roi_image, roi_image, COLOR_BGR2HSV);
        inRange(roi_image, Scalar(73, 80, 175), Scalar(107, 255, 255), roi_image);
        // 将roi区域转换为32x32的图像
        resize(roi_image, roi_image, Size(32, 32));


        // 将图像转换为1x1x32x32的图像
        Mat input_blob = dnn::blobFromImage(roi_image, 1.0, Size(32, 32), Scalar(0), true, false);
        normalize(input_blob, input_blob, -1.0, 1.0, NORM_MINMAX);

        // 获取输入数据指针
        float* input_data = input_blob.ptr<float>();

        // 将输入数据复制到OpenVINO张量
        auto input_data_ptr = input_tensor.data<float>();
        std::memcpy(input_data_ptr, input_data, input_blob.total() * input_blob.elemSize());

        // 进行推断
        infer_request.infer();

        //输出
        auto output = infer_request.get_output_tensor();
        const float* output_buffer = output.data<float>();
        //输出类别的概率
        int pred, max = -100;
        for (int i = 0; i < output.get_size(); i++)
        {
            if (output_buffer[i] > max)
            {
                max = output_buffer[i];
                pred = i;
            }
        }
        if(pred==0)
        {
            //用红色绘制出旋转矩形
            Point2f points[4];
            finalRects[i].points(points);
            for(int j=0;j<4;j++)
            {
                line(roi_img,points[j],points[(j+1)%4],Scalar(0,0,255),2);
            }
            //计算旋转矩形的最大外接矩形
            Point2f center = finalRects[i].center;
            float angle = finalRects[i].angle;
            float width = finalRects[i].size.width;
            float height = finalRects[i].size.height;
            Point2f rect_points[4];
            finalRects[i].points(rect_points);
            float x_min = 10000,x_max = 0,y_min = 10000,y_max = 0;
            for(int j=0;j<4;j++)
            {
                if(rect_points[j].x<x_min)
                x_min = rect_points[j].x;
                if(rect_points[j].x>x_max)
                x_max = rect_points[j].x;
                if(rect_points[j].y<y_min)
                y_min = rect_points[j].y;
                if(rect_points[j].y>y_max)
                y_max = rect_points[j].y;
            }
            //截取矩形区域
            Rect rect(x_min,y_min,x_max-x_min,y_max-y_min);
            //保存左上角点
            Point2f left_top = Point2f(x_min,y_min);
            Mat roiimage=roi_img.clone();
            Mat roi_rect = roiimage(rect);
            cvtColor(roi_rect,roi_rect,COLOR_BGR2HSV);
            inRange(roi_rect, Scalar(73, 80, 175), Scalar(107, 255, 255), roi_rect);
            
            floodFill(roi_rect,Point2f(0,0),Scalar(255));
            //反色
            bitwise_not(roi_rect,roi_rect);
            //使用矩形拟合
            vector<vector<Point>> contours;
            findContours(roi_rect,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE);
            for(int i=0;i<contours.size();i++)
            {
                //筛选掉过小的轮廓
                if(contourArea(contours[i])<50)
                {
                    contours.erase(contours.begin()+i);
                    i--;
                }
            }
            //使用旋转矩形拟合
            vector<RotatedRect> rotatedRects;
            for(int j=0;j<contours.size();j++)
            {
                RotatedRect rotatedRect = minAreaRect(contours[j]);
                rotatedRects.push_back(rotatedRect);
            }
            //画出最终的矩形
            for(int j=0;j<rotatedRects.size();j++)
            {
                Point2f vertices[4];
                rotatedRects[j].points(vertices);
                for(int k=0;k<4;k++)
                {
                    //筛选出长宽比1：1的矩形,画出每个旋转矩形的中心点
                    if(abs(rotatedRects[j].size.width/rotatedRects[j].size.height)<1.2&&abs(rotatedRects[j].size.width/rotatedRects[j].size.height)>0.8){
                    line(roi_img,vertices[k]+left_top,vertices[(k+1)%4]+left_top,Scalar(0,255,0),2);
                    circle(roi_img,rotatedRects[j].center+left_top,2,Scalar(0,0,255),2);
                    //使用旋转矩形的左下角点和角度，创建一个1/2长度的矩形
                    
                    }
                 }
            }
            //解算旋转矩形的中心点
        
        }

    }
    return roi_img;
}