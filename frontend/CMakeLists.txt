# frontend/CMakeLists.txt
# 前端Qt项目的CMake构建脚本，配置Qt、编译选项和安装规则。
cmake_minimum_required(VERSION 3.16)

project(frontend LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)      # 自动处理MOC（元对象编译）
set(CMAKE_AUTORCC ON)      # 自动处理Qt资源文件
set(CMAKE_AUTOUIC ON)      # 自动处理UI文件

# 指定Qt安装路径
set(CMAKE_PREFIX_PATH "C:/MyQt/6.5.3/msvc2019_64")

# MSVC下强制__cplusplus正确
if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /utf-8)
endif()

add_definitions(-D_WIN32_WINNT=0x0601) # 指定Windows最低版本

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network) # 查找Qt6组件

add_executable(frontend
    main.cpp
    MainWindow.cpp
    NetworkClient.cpp
)

target_link_libraries(frontend PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Network) # 链接Qt库

install(TARGETS frontend
    RUNTIME DESTINATION bin)

install(FILES MainWindow.h NetworkClient.h
    DESTINATION include/frontend)
