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