cmake_minimum_required(VERSION 3.23)
project(Jsonpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Jsonpp SHARED lexer.cpp json.cpp)
