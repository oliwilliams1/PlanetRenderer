cmake_minimum_required(VERSION 3.12)

project(DistanceTransform CXX)

find_package(SDL2 CONFIG REQUIRED)

add_executable(DistanceTransform "main.cpp")

# Correctly link the SDL2 library
target_link_libraries(DistanceTransform SDL2::SDL2)

# Set C++ standard
set_property(TARGET DistanceTransform PROPERTY CXX_STANDARD 20)
set_property(TARGET DistanceTransform PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DistanceTransform PROPERTY CXX_EXTENSIONS OFF)