#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

// 定义枚举类型
enum class InstructionType
{
    LogicKeyword,
    FunctionKeyword,
    Separator,
    Unknown,
    Constant,
    Illegal
};

enum class FunctionKeywords
{
    none,
    print,
    input
};

enum class LogicKeywords
{
    none,
    AND,
    OR,
    NOT,
    set,
    min
};

struct Logic
{
    std::vector<std::shared_ptr<bool>> values;
    std::unordered_set<int> mins;

    // 将values中的bool值组合成二进制数并转换为十进制，并判断存在于mins中
    bool isValueInMins() const;
};

class Interpreter
{
public:
    // 构造函数，接受std::vector<std::string>作为参数
    Interpreter(const std::vector<std::string> &instructions);
    Interpreter(std::string &filename);
    Interpreter(const Interpreter &) = delete;
    Interpreter &operator=(const Interpreter &) = delete;
    ~Interpreter();

    // 运行解释器
    void Run();

    // 单步执行
    void Step();

    // 获取运行时间
    std::string getRunDuration() const;

private:
    std::string filename;

    std::vector<std::string> instructions;
    size_t currentInstruction;

    // 行号
    int line;

    std::vector<std::vector<std::string>> GetLogicWords(std::vector<std::string> &currentLine);

    // 逻辑
    bool LogicProcessing(std::vector<std::vector<std::string>> &words);

    // 处理逻辑关键字
    static const std::unordered_set<std::string> logicKeywords;
    // 处理函数关键字
    static const std::unordered_set<std::string> functionKeywords;

    // 变量池
    std::unordered_map<std::string, std::shared_ptr<bool>> variablePool;
    // 逻辑池
    std::unordered_map<std::string, Logic> LogicPool;

    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    std::string run_duration_;

    // 检查一个字符串是否满足C++变量的命名规则
    bool isValidCppIdentifier(const std::string &identifier);
    // 将字符串转换为布尔值
    bool convertToBool(const std::string &str);

    /// @brief  报告错误
    void reportError(const std::string &errorMessage, int line) const;
    /// @brief  报告警告
    void reportWarning(const std::string &warningMessage, int line) const;
};

#endif // INTERPRETER_H