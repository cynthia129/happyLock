add_library(model STATIC
    Document.cpp
    Document.h
    DocumentRepository.cpp
    DocumentRepository.h
    Version.cpp
    Version.h
)

target_include_directories(model PUBLIC "C:/sqlite3")
target_link_libraries(model PRIVATE "C:/sqlite3/sqlite3.lib")

install(TARGETS model
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(FILES Document.h DocumentRepository.h Version.h
    DESTINATION include/model) 