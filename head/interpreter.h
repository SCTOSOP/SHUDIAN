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

    // 运行解释器
    void Run();

    // 单步执行
    void Step();

private:
    std::vector<std::string>
        instructions;
    size_t currentInstruction;
    int line; // 行号

    // 返回当前指令的类型
    InstructionType getInstructionType(const std::string &instruction) const;

    // 关键字集合
    static const std::unordered_set<std::string>
        logicKeywords;
    static const std::unordered_set<std::string>
        functionKeywords;

    // 变量池
    std::unordered_map<std::string, bool>
        variablePool;

    // 处理未知类型的指令
    void handleUnknownInstruction(const std::string &instruction);

    bool convertToBool(const std::string &str);
};

#endif // INTERPRETER_H