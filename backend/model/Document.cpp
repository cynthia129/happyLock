#include "Document.h"

Document::Document(int id, const std::string& title, const std::string& content, int version)
    : id(id), title(title), content(content), version(version) {} 