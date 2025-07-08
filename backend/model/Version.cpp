#include "Version.h"

// 带参数构造函数，初始化版本的ID、所属文档ID、内容、作者和时间戳
Version::Version(int id, int docId, const std::string& content, const std::string& author, std::time_t timestamp)
    : id(id), docId(docId), content(content), author(author), timestamp(timestamp)
    {

    }