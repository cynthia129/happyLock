#pragma once
#include <string>

// Document类表示一个文档对象，包含文档的ID、标题、内容和版本号。
class Document {
public:
    int id;                // 文档唯一标识符
    std::string title;     // 文档标题
    std::string content;   // 文档内容
    int version;           // 当前文档版本号

    // 默认构造函数，创建空文档对象
    Document() = default;
    // 带参数构造函数，初始化所有成员变量
    Document(int id, const std::string& title, const std::string& content, int version);
}; 