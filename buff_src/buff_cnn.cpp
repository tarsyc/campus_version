#include <opencv2/opencv.hpp>
#include <iostream> 
#include <openvino/openvino.hpp>    
#include "Buff.h"

using namespace std;
using namespace cv;
using namespace ov;

int Buff::buff_recognize(Mat frame)
{
    
    Core core;
    CompiledModel model = core.compile_model("..config/buff.onnx", "CPU");
    InferRequest infer_request = model.create_infer_request();
    Tensor input_tensor = infer_request.get_input_tensor(0);
    Shape tensor_shape = input_tensor.get_shape();
    
    
}