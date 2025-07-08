# backend/service/CMakeLists.txt
# service模块构建脚本，编译协同服务相关代码。
add_library(service STATIC
    CollaborationService.cpp
    CollaborationService.h
)

target_include_directories(service PUBLIC .)
target_include_directories(service PUBLIC "C:/json/json-develop/include") # 包含json头文件
target_compile_definitions(service PRIVATE _WIN32_WINNT=0x0A00)

install(TARGETS service
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(FILES CollaborationService.h
    DESTINATION include/service) 