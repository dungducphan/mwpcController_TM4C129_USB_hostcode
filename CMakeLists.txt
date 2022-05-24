cmake_minimum_required(VERSION 3.22)
project(host_usb_code)

set(CMAKE_CXX_STANDARD 17)

add_executable(host_usb_code main.cpp)
target_include_directories(host_usb_code PRIVATE ~/apps/ti/ccs1120/ccs/ccs_base/cloudagent/src/node_modules/usb/libusb/libusb)
target_link_libraries(host_usb_code /home/dphan/apps/ti/ccs1120/ccs/ccs_base/common/bin/libusb-1.0.so)
