cmake_minimum_required(VERSION 3.0)
project(RecognizeProject)

# 设置编译标准
set(CMAKE_CXX_STANDARD 14)

set(SOURCES
    src/img_process.cpp
    src/find_contours.cpp
    src/armors_select.cpp
    src/cnn_recognize.cpp
    src/pose_estimation.cpp
    main.cpp
)

# 查找 OpenCV 包
find_package(OpenCV 4.8 REQUIRED)

# 查找 OpenVINO
find_package(OpenVINO REQUIRED COMPONENTS Runtime)

# 查找 YAML-CPP 包
find_package(yaml-cpp REQUIRED)

# 添加可执行文件并指定源文件，包括 main.cpp 
add_executable(recognize main.cpp 
    src/img_process.cpp 
    src/find_contours.cpp 
    src/armors_select.cpp 
    src/draw_armors.cpp 
    src/cnn_recognize.cpp 
    src/pose_estimation.cpp
    )

# 包含头文件路径，包括 OpenCV 和 YAML-CPP 的头文件
include_directories(${OpenCV_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS} include)

# 链接 OpenCV、OpenVINO Runtime 和 YAML-CPP 库
target_link_libraries(recognize ${OpenCV_LIBS} openvino::runtime ${YAML_CPP_LIBRARIES})
