# Sets the minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Defines the project name and programming language
project(Chava LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Creates an executable target named "MyApp" from main.cpp
add_executable(chava main.cpp)

