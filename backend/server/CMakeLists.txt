# backend/server/CMakeLists.txt
# server模块构建脚本，编译服务器主逻辑。
add_library(serverlib STATIC
    server.cpp
    server.h
    session.cpp
    session.h
)

target_include_directories(serverlib PUBLIC . "C:/json/json-develop/include" "C:/sqlite3") # 包含json和sqlite3头文件

target_link_libraries(serverlib
    PRIVATE
        Boost::system
        model
        service
        "C:/sqlite3/sqlite3.lib"
)

install(TARGETS serverlib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(FILES server.h session.h
    DESTINATION include/server)

target_compile_definitions(serverlib PRIVATE _WIN32_WINNT=0x0A00)

add_executable(happyLockServer main.cpp)
target_link_libraries(happyLockServer PRIVATE serverlib Boost::system model service)
target_compile_definitions(happyLockServer PRIVATE _WIN32_WINNT=0x0A00)

install(TARGETS happyLockServer
    RUNTIME DESTINATION bin)