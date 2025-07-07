set(Boost_DEBUG ON)
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

set(BOOST_ROOT "C:/boost_1_88_0-bin-msvc-all-32-64/boost_1_88_0")
set(CMAKE_PREFIX_PATH "C:/MyQt/Tools/mingw1310_64")

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${BOOST_ROOT})

add_subdirectory(model)
add_subdirectory(service)
add_subdirectory(server)

add_library(sessionlib STATIC
    session.cpp
    session.h
)

target_include_directories(sessionlib PUBLIC . "C:/sqlite3")

target_link_libraries(sessionlib
    PRIVATE
        Boost::system
        model
        service
        serverlib
        "C:/sqlite3/sqlite3.lib"
) 