cmake_minimum_required(VERSION 3.0)
project(RecognizeProject)


set(CMAKE_CXX_STANDARD 14)

set(SOURCES
    armor_src/img_process.cpp
    armor_src/find_contours.cpp
    armor_src/armors_select.cpp
    armor_src/cnn_recognize.cpp
    armor_src/pose_estimation.cpp
    armor_src/decision_makeing.cpp
    armor_src/kalamn_filter.cpp
    buff_src/buff_detect.cpp
    buff_src/buff_cnn.cpp
    buff_src/find_r.cpp
    buff_src/buff_predict.cpp
    buff_src/buff_pnp.cpp
    main.cpp
)

find_package(OpenCV 4.8 REQUIRED)
find_package(OpenVINO REQUIRED COMPONENTS Runtime)
find_package(yaml-cpp REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})



add_executable(recognize main.cpp 
    armor_src/img_process.cpp 
    armor_src/find_contours.cpp 
    armor_src/armors_select.cpp 
    armor_src/draw_armors.cpp 
    armor_src/cnn_recognize.cpp 
    armor_src/pose_estimation.cpp
    armor_src/decision_makeing.cpp
    armor_src/kalamn_filter.cpp
    buff_src/buff_detect.cpp
    buff_src/buff_cnn.cpp
    buff_src/find_r.cpp
    buff_src/buff_predict.cpp
    buff_src/buff_pnp.cpp
    )


include_directories(${OpenCV_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS} include/Armor include/Buff include usr/include)


target_link_libraries(recognize ${OpenCV_LIBS} openvino::runtime ${YAML_CPP_LIBRARIES} Eigen3::Eigen)
