#pragma once
#include <string>
#include <ctime>

// Version类表示文档的一个历史版本，包含版本ID、所属文档ID、内容、作者和时间戳。
class Version {
public:
    int id;                // 版本唯一标识符
    int docId;             // 所属文档的ID
    std::string content;   // 该版本的文档内容
    std::string author;    // 编辑该版本的作者
    std::time_t timestamp; // 版本创建时间戳

    // 默认构造函数，创建空版本对象
    Version() = default;
    // 带参数构造函数，初始化所有成员变量
    Version(int id, int docId, const std::string& content, const std::string& author, std::time_t timestamp);
}; 