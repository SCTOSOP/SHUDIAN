#ifndef FILE_H
#define FILE_H

#include <vector>
#include <string>

class File
{
public:
    // 构造函数，接受文件路径字符串
    File(const std::string &filePath);

    // 读取文件全部内容
    std::string readFile() const;

    // 对文件内容进行分割，在空格、换行、制表符时分割，返回字符串数组
    std::vector<std::string> splitContent(const std::string &content) const;

private:
    std::string filePath;
};

#endif // FILE_H