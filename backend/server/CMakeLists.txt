add_library(serverlib STATIC
    server.cpp
    server.h
    session.cpp
    session.h
)

target_include_directories(serverlib PUBLIC . "C:/json/json-develop/include" "C:/sqlite3")
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

add_executable(happyLockServer main.cpp)
target_link_libraries(happyLockServer PRIVATE serverlib Boost::system model service)

install(TARGETS happyLockServer
    RUNTIME DESTINATION bin)