#include "Version.h"

Version::Version(int id, int docId, const std::string& content, const std::string& author, std::time_t timestamp)
    : id(id), docId(docId), content(content), author(author), timestamp(timestamp) {} 