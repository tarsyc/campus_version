#include "Armor.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <openvino/openvino.hpp>

using namespace std;
using namespace cv;
using namespace ov;

int Armor::cnn_recognize(Mat& frame)
{
    
    //使用旋转矩形的中心点和长宽计算出装甲板的四个顶点
    cv::Point2f vertices[4];
    cv::RotatedRect roi_rect = cv::RotatedRect(center, cv::Size2f(width, height), angle);
    roi_rect.points(vertices);
    //将装甲板的四个顶点存储到vector中
    cv::Point2f roi_points[4];
    roi_points[0]=vertices[0];
    roi_points[1]=vertices[1];
    roi_points[2]=vertices[2];
    roi_points[3]=vertices[3];
    cv::Point2f dst[4];
    dst[0]=cv::Point2f(0,0);
    dst[1]=cv::Point2f(width,0);
    dst[2]=cv::Point2f(width,height);
    dst[3]=cv::Point2f(0,height);
    // 计算透视变换矩阵
    cv::Mat M = cv::getPerspectiveTransform(roi_points, dst);
    // 进行透视变换
    cv::Mat roi_warp;
    cv::warpPerspective(frame, roi_warp, M, cv::Size(width, height));
    cv::namedWindow("roi",cv::WINDOW_NORMAL);
    cv::imshow("roi",roi_warp);
    // 创建OpenVINO的核心对象
    ov::Core core;
    // 编译模型
    ov::CompiledModel model = core.compile_model("../config/number_recognize.onnx", "CPU");
    // 创建推断请求
    ov::InferRequest infer_request = model.create_infer_request();
    // 获取输入张量
    ov::Tensor input_tensor = infer_request.get_input_tensor(0);
    ov::Shape tensor_shape = input_tensor.get_shape();
    // 定义用于转换的M矩阵
    // 将输入图像进行透视变换
    // 将roi区域转换为灰度图
    cv::cvtColor(roi_warp, roi_warp, cv::COLOR_BGR2GRAY);
    // 将roi区域转换为28x28的图像
    cv::resize(roi_warp, roi_warp, cv::Size(28, 28));
    //二值化
    cv:: threshold(roi_warp, roi_warp, 50, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    // 将roi区域转换为1x1x28x28的图像
    cv::Mat input_blob = cv::dnn::blobFromImage(roi_warp, 1.0, cv::Size(28, 28), cv::Scalar(0), true, false);

    cv::normalize(input_blob, input_blob, -1.0, 1.0, cv::NORM_MINMAX);
    // 获取输入数据指针
    // 获取输入数据指针
    float* input_data = input_blob.ptr<float>();

    // 将输入数据复制到OpenVINO张量
    auto input_data_ptr = input_tensor.data<float>();
    std::memcpy(input_data_ptr, input_data, input_blob.total() * input_blob.elemSize());


    // 进行推断
    infer_request.infer();

    auto output =infer_request.get_output_tensor();
    const float* output_buffer = output.data<float>();
    int pred,max=-100;
    for(int i=0;i<output.get_size();i++)
    {
        if(output_buffer[i]>max)
        {
            max=output_buffer[i];
            pred=i+1;
        }
    }
    return pred;
}
