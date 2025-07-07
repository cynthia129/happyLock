add_library(service STATIC
    CollaborationService.cpp
    CollaborationService.h
)

target_include_directories(service PUBLIC .)
target_include_directories(service PUBLIC "C:/json/json-develop/include")

install(TARGETS service
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(FILES CollaborationService.h
    DESTINATION include/service) 