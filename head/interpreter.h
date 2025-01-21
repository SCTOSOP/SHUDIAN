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
    NOT
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

    std::vector<std::string>
        instructions;
    size_t currentInstruction;
    int line; // 行号

    // 返回当前指令的类型
    InstructionType getInstructionType(const std::string &instruction) const;

    // 处理逻辑关键字
    static const std::unordered_set<std::string>
        logicKeywords;
    // 处理函数关键字
    static const std::unordered_set<std::string>
        functionKeywords;
    // 检查一个字符串是否满足C++变量的命名规则
    bool isValidCppIdentifier(const std::string &identifier);

    // 变量池
    std::unordered_map<std::string, bool> variablePool;

    // 处理未知类型的指令
    void AddTheValueToThePool(const std::string &instruction);

    // 将字符串转换为布尔值
    bool convertToBool(const std::string &str);

    /// @brief  报告错误
    void reportError(const std::string &errorMessage, int line) const;

    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    std::string run_duration_;
};

#endif // INTERPRETER_H