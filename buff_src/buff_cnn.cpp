#include <opencv2/opencv.hpp>
#include <iostream> 
#include <openvino/openvino.hpp>    
#include "Buff.h"
#include "Buff_manage.h"

using namespace std;
using namespace cv;
using namespace ov;

int Buff_manage::buff_cnn(Mat hsv, RotatedRect rect)
{
    //传入图像为hsv图像
    //对传入图像进行处理
    Mat roi;//截取的部分
    Mat rotationMatrix=getRotationMatrix2D(rect.center,rect.angle,1.0);
    warpAffine(hsv,roi,rotationMatrix,hsv.size());
    Rect roi_rect(rect.center.x-rect.size.width/2,rect.center.y-rect.size.height/2,rect.size.width,rect.size.height);
    roi = hsv(roi_rect);
    resize(roi,roi,Size(32,32));
    Core core;
    CompiledModel model = core.compile_model("../config/nn.onnx", "CPU");
    InferRequest infer_request = model.create_infer_request();
    Tensor input_tensor = infer_request.get_input_tensor(0);
    Shape tensor_shape = input_tensor.get_shape();
    Mat input_blob=dnn::blobFromImage(roi,1.0,Size(32,32),Scalar(0),true,false);
    normalize(input_blob,input_blob,-1.0,1.0,NORM_MINMAX);
    float* input_data=input_blob.ptr<float>();
    auto input_data_ptr=input_tensor.data<float>();
    std::memcpy(input_data_ptr,input_data,input_blob.total()*input_blob.elemSize());
    infer_request.infer();
    auto output=infer_request.get_output_tensor();
    const float* output_buffer=output.data<float>();
    int pred,max=-100;
    for(int i=0;i<output.get_size();i++)
    {
        if(output_buffer[i]>max)
        {
            max=output_buffer[i];
            pred=i;
        }
    }
    return pred;
}