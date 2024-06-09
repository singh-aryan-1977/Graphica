cmake_minimum_required(VERSION 3.27)
project(Graphica)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Graphica
        Main_Classes/main.cpp
        Header_Files/vec3.h
        Header_Files/color.h
        Header_Files/ray.h
        Header_Files/entity.h
        Header_Files/sphere.h
        Header_Files/entity_list.h
        Header_Files/constants.h
        Header_Files/interval.h
        Header_Files/camera.h
        Header_Files/material.h
        Header_Files/triangular_prism.h
        Header_Files/ctpl.h
        Header_Files/bs_thread_pool.h
        Header_Files/bs_thread_pool_utils.h
        Header_Files/pyramid.h
)
