#include "Document.h"

// 带参数构造函数，初始化文档的ID、标题、内容和版本号
Document::Document(int id, const std::string& title, const std::string& content, int version)
    : id(id), title(title), content(content), version(version) {} 