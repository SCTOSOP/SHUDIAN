#include <fstream>
#include <sstream>
#include <stdexcept>
#include "file.h"

// 构造函数，接受文件路径字符串
File::File(const std::string &filePath) : filePath(filePath) {}

// 读取文件全部内容
std::string File::readFile() const
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("无法打开文件: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 对文件内容进行分割，在空格、换行、制表符时分割，返回字符串数组
std::vector<std::string> File::splitContent(const std::string &content) const
{
    std::vector<std::string> result;
    std::istringstream iss(content);
    std::string token;
    while (iss >> token)
    {
        result.push_back(token);
    }
    return result;
}
