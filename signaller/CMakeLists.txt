cmake_minimum_required(VERSION 3.20)
project(popham_signaller)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(popham_signaller ${PROJECT_SOURCE_DIR}/src/popham_signaller.cpp)



