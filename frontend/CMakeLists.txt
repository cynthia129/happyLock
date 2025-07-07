cmake_minimum_required(VERSION 3.16)

project(frontend LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 指定Qt安装路径
set(CMAKE_PREFIX_PATH "C:/MyQt/6.5.3/msvc2019_64")

# MSVC下强制__cplusplus正确
if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /utf-8)
endif()

add_definitions(-D_WIN32_WINNT=0x0601)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network)

add_executable(frontend
    main.cpp
    MainWindow.cpp
    NetworkClient.cpp
)

target_link_libraries(frontend PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Network)
