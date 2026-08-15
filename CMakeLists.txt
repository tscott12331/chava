cmake_minimum_required(VERSION 3.15)

project(Chava LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chava 
    main.cpp
    tokenizer.cpp
)

