cmake_minimum_required(VERSION 3.27)
project(Graphica)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Graphica
        main.cpp
        Header_Files/vec3.h
        Header_Files/color.h
        Header_Files/ray.h
)
