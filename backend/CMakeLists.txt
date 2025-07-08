# backend/CMakeLists.txt
# 后端C++服务端构建脚本，配置Boost、sqlite3、子模块和安装规则。
set(Boost_DEBUG ON) # 启用Boost调试输出
set(Boost_NO_SYSTEM_PATHS ON) # 不使用系统路径查找Boost
set(Boost_USE_STATIC_LIBS        ON) # 使用静态链接Boost
set(Boost_USE_MULTITHREADED      ON) # 启用多线程
set(Boost_USE_STATIC_RUNTIME    OFF) # 不使用静态运行时

set(BOOST_ROOT "C:/boost_1_88_0-bin-msvc-all-32-64/boost_1_88_0") # 指定Boost根目录
set(CMAKE_PREFIX_PATH "C:/MyQt/Tools/mingw1310_64") # 指定Qt路径

find_package(Boost REQUIRED COMPONENTS system) # 查找Boost system库
include_directories(${BOOST_ROOT}) # 包含Boost头文件

add_subdirectory(model)    # 添加model子目录
add_subdirectory(service)  # 添加service子目录
add_subdirectory(server)   # 添加server子目录

add_library(sessionlib STATIC
    session.cpp
    session.h
)

target_include_directories(sessionlib PUBLIC . "C:/sqlite3") # 包含sqlite3头文件
target_compile_definitions(sessionlib PRIVATE _WIN32_WINNT=0x0A00)

target_link_libraries(sessionlib
    PRIVATE
        Boost::system
        model
        service
        serverlib
        "C:/sqlite3/sqlite3.lib"
)

install(TARGETS sessionlib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(FILES session.h
    DESTINATION include) 