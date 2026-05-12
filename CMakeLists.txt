cmake_minimum_required(VERSION 3.10)
project(RaylibAnimSystem)

set(CMAKE_CXX_STANDARD 17)

#FIND  packages installed via pacman
find_package(fmt REQUIRED)
find_package(raylib REQUIRED)

add_executable(AnimSystem main.cpp)

#Link libraries
target_link_libraries(AnimSystem fmt::fmt  raylib)

#For windows/MinGW we dont' get a console window in release if desired
# set_target_properties(AnimSystem PROPERTIES WIN32_EXECUTABLE TRUE)
