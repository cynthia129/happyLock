add_library(service STATIC
    CollaborationService.cpp
    CollaborationService.h
)

target_include_directories(service PUBLIC .)
target_include_directories(service PUBLIC "C:/json/json-develop/include") 