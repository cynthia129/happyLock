#pragma once
#include <string>

class Document {
public:
    int id;
    std::string title;
    std::string content;
    int version;

    Document() = default;
    Document(int id, const std::string& title, const std::string& content, int version);
}; 