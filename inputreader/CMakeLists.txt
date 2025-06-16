cmake_minimum_required(VERSION 3.15)
project(inputreader)

set(CMAKE_CXX_STANDARD 14)

add_executable(inputreader main.cpp
        mangmi/utils/MangmiIntercepter.cpp
        mangmi/MangmiFlinger.cpp mangmi/MangmiFlinger.h
        mangmi/MangmiPolicy.cpp mangmi/MangmiPolicy.h mangmi/Macro.h " include/EventHub.h"
        mangmi/utils/MiThreadPool.cpp  mangmi/utils/MiThreadPool.h
        mangmi/utils/MangmiSocketClient.cpp mangmi/utils/MangmiSocketClient.h
        mangmi/utils/MangmiUtils.cpp  mangmi/utils/MangmiUtils.h
        mangmi/json/jsoncpp.cpp mangmi/InputFilter.cpp mangmi/InputFilter.h
#        mangmi/extend/ThreadPool.cpp mangmi/extend/ThreadPool.h
        mangmi/utils/MangmiConfig.cpp mangmi/utils/MangmiConfig.h)