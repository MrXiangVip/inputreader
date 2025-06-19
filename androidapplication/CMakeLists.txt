cmake_minimum_required(VERSION 3.15)
project(androidapplication)

set(CMAKE_CXX_STANDARD 14)

add_executable(androidapplication main.cpp AndroidClient.cpp AndroidClient.h AndroidServer.cpp AndroidServer.h)