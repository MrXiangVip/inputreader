cmake_minimum_required(VERSION 3.17)
project(monitor)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)

# 设置编译选项，将警告视为错误
set(CMAKE_CXX_FLAGS "$ENV{CXX_FLAGS} -Werror")

add_executable(${PROJECT_NAME} main.cpp
        window/mainwindow.cpp window/mainwindow.ui
        mangmi/MangmiEvent.cpp mangmi/MangmiPolicy.cpp
        mangmi/utils/MiThreadPool.cpp  mangmi/utils/MiThreadPool.h
        mangmi/utils/MangmiSocketClient.cpp mangmi/utils/MangmiSocketClient.h
        mangmi/utils/MangmiUtils.cpp  mangmi/utils/MangmiUtils.h
        mangmi/json/jsoncpp.cpp mangmi/InputFilter.cpp mangmi/InputFilter.h
        mangmi/utils/MangmiConfig.cpp mangmi/utils/MangmiConfig.h
        mangmi/utils/MangmiSocketServer.cpp mangmi/utils/MangmiSocketServer.h
        mangmi/utils/MangmiIntercepter.cpp
        )

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})

