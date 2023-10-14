cmake_minimum_required(VERSION 3.0)
project(RecognizeProject)


set(CMAKE_CXX_STANDARD 14)

set(SOURCES
    src/img_process.cpp
    src/find_contours.cpp
    src/armors_select.cpp
    src/cnn_recognize.cpp
    src/pose_estimation.cpp
    src/decision_makeing.cpp
    src/kalamn_filter.cpp
    main.cpp
)

find_package(OpenCV 4.8 REQUIRED)
find_package(OpenVINO REQUIRED COMPONENTS Runtime)
find_package(yaml-cpp REQUIRED)


add_executable(recognize main.cpp 
    src/img_process.cpp 
    src/find_contours.cpp 
    src/armors_select.cpp 
    src/draw_armors.cpp 
    src/cnn_recognize.cpp 
    src/pose_estimation.cpp
    src/decision_makeing.cpp
    src/kalamn_filter.cpp
    )


include_directories(${OpenCV_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS} include)


target_link_libraries(recognize ${OpenCV_LIBS} openvino::runtime ${YAML_CPP_LIBRARIES})
