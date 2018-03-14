cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++11 HAS_STD_CPP11_FLAG)
if(HAS_STD_CPP11_FLAG)
  add_compile_options(-std=c++11)
endif()

project(BagFromImages)

rosbuild_init()

FIND_PACKAGE(OpenCV REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

rosbuild_add_executable(${PROJECT_NAME}
main.cpp
Thirdparty/DLib/FileFunctions.cpp)

target_link_libraries (${PROJECT_NAME}
${OpenCV_LIBS})
