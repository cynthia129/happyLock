cmake_minimum_required(VERSION 3.10)

project(happyLock LANGUAGES CXX)

# 可选：设置全局C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加后端和前端子目录
add_subdirectory(backend)
add_subdirectory(frontend)