#pragma once
#include <string>
#include <ctime>

class Version {
public:
    int id;
    int docId;
    std::string content;
    std::string author;
    std::time_t timestamp;

    Version() = default;
    Version(int id, int docId, const std::string& content, const std::string& author, std::time_t timestamp);
}; 