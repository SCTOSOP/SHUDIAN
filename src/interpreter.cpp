#include "interpreter.h"
#include "file.h"
#include <iostream>

// 初始化关键字集合
const std::unordered_set<std::string> Interpreter::logicKeywords = {"and", "or", "not"};
const std::unordered_set<std::string> Interpreter::functionKeywords = {"print", "input"};

void CheckTheValueToVector(std::vector<std::string> &vec, std::string &str)
{
    if (str == "" || str == " ")
    {
        return;
    }
    vec.push_back(str);
}

// 构造函数，接受std::vector<std::string>作为参数
Interpreter::Interpreter(const std::vector<std::string> &instructions)
    : instructions(instructions), currentInstruction(0), line(1) {}

Interpreter::Interpreter(std::string &filename)
    : Interpreter(File(filename).splitContent(File(filename).readFile()))
{
    this->filename = filename;
}

Interpreter::~Interpreter()
{
    instructions.clear();
    variablePool.clear();
}

// 运行解释器
void Interpreter::Run()
{
    start_time_ = std::chrono::high_resolution_clock::now();
    while (currentInstruction < instructions.size())
    {
        Step();
    }
    end_time_ = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_ - start_time_).count();
    run_duration_ = std::to_string(duration);
}

// 单步执行
void Interpreter::Step()
{
    std::vector<std::string> currentLine;
    for (currentInstruction; currentInstruction < instructions.size(); currentInstruction++)
    {
        if (instructions[currentInstruction] == ";")
        {
            currentInstruction++;
            break;
        }
        currentLine.push_back(instructions[currentInstruction]);
    }
    if (currentLine.empty())
    {
        return;
    }

    std::string left_;
    std::vector<std::string> right_;
    FunctionKeywords func = FunctionKeywords::none;
    LogicKeywords lgs = LogicKeywords::none;

    size_t currentNumInLine = 0;
    while (currentNumInLine < currentLine.size())
    {
        std::string instruction = currentLine[currentNumInLine];
        InstructionType type = getInstructionType(instruction);
        // std::cout << "Executing: " << instruction << " (Type: " << static_cast<int>(type) << ")" << std::endl;

        switch (type)
        {
        case InstructionType::LogicKeyword:
            // 处理逻辑关键字
            if (instruction == "and")
            {
                lgs = LogicKeywords::AND;
            }
            else if (instruction == "or")
            {
                lgs = LogicKeywords::OR;
            }
            else if (instruction == "not")
            {
                lgs = LogicKeywords::NOT;
            }
            break;
        case InstructionType::FunctionKeyword:
            // 处理函数关键字
            if (instruction == "print")
            {
                func = FunctionKeywords::print;
                left_ = "PRINT";
            }
            else if (instruction == "input")
            {
                func = FunctionKeywords::input;
                right_.push_back("-");
            }
            break;
        case InstructionType::Unknown:
            // 处理用户变量
            if (left_.empty())
            {
                AddTheValueToThePool(instruction);
                left_ = instruction;
            }
            else
            {
                if (variablePool.find(instruction) == variablePool.end())
                {
                    reportError("Undefined variable used as right value", line);
                    throw std::runtime_error("Undefined variable used as right value");
                }
                right_.push_back(instruction);
            }
            break;
        case InstructionType::Constant:
            // 处理常量
            if (func == FunctionKeywords::none && left_.empty())
            {
                reportError("Constant cannot be used as left value", line);
                throw std::runtime_error("Constant cannot be used as left value");
            }
            right_.push_back(instruction);
            break;
        case InstructionType::Illegal:
            // 处理错误
            reportError("Illegal instruction encountered", line);
            throw std::runtime_error("Illegal instruction encountered");
            break;
        }

        currentNumInLine++;
    }

    if (right_.empty())
    {
        reportError("Right value should not be empty in a statement", line);
        throw std::runtime_error("Right value should not be empty in a statement");
    }

    bool tmp = false;
    switch (lgs)
    {
    case LogicKeywords::none:
        break;
    case LogicKeywords::AND:
        tmp = convertToBool(right_[0]);
        for (size_t i = 1; i < right_.size(); i++)
        {
            tmp = tmp && convertToBool(right_[i]);
        }
        right_.clear();
        right_.push_back(std::to_string(tmp));
        break;
    case LogicKeywords::OR:
        tmp = convertToBool(right_[0]);
        for (size_t i = 1; i < right_.size(); i++)
        {
            tmp = tmp || convertToBool(right_[i]);
        }
        right_.clear();
        right_.push_back(std::to_string(tmp));
        break;
    case LogicKeywords::NOT:
        tmp = !convertToBool(right_[0]);
        right_.clear();
        right_.push_back(std::to_string(tmp));
        break;
    }

    switch (func)
    {
    case FunctionKeywords::none:
        variablePool[left_] = convertToBool(right_[0]);
        break;
    case FunctionKeywords::print:
        for (const std::string &word : right_)
        {
            std::cout << word << " = " << variablePool[word] << std::endl;
        }
        break;
    case FunctionKeywords::input:
        auto pause_time = std::chrono::high_resolution_clock::now();
        std::string tmp_input = "0";
        std::cout << "\033[42m请输入变量 " << left_ << " :\033[0m";
        std::cin >> tmp_input;
        auto resume_time = std::chrono::high_resolution_clock::now();
        variablePool[left_] = convertToBool(tmp_input);
        start_time_ += (resume_time - pause_time); // Adjust start_time to exclude input time
        break;
    }
}

std::string Interpreter::getRunDuration() const
{
    return run_duration_;
}

// 返回当前指令的类型
InstructionType Interpreter::getInstructionType(const std::string &instruction) const
{
    if (logicKeywords.find(instruction) != logicKeywords.end())
    {
        return InstructionType::LogicKeyword;
    }
    if (functionKeywords.find(instruction) != functionKeywords.end())
    {
        return InstructionType::FunctionKeyword;
    }
    if (instruction == "1" || instruction == "0")
    {
        return InstructionType::Constant;
    }
    if (std::all_of(instruction.begin(), instruction.end(), ::isdigit) && instruction != "0" && instruction != "1")
    {
        return InstructionType::Illegal;
    }
    return InstructionType::Unknown;
}

bool Interpreter::isValidCppIdentifier(const std::string &identifier)
{
    if (identifier.empty())
    {
        return false;
    }

    // 检查第一个字符是否为字母或下划线
    if (!std::isalpha(identifier[0]) && identifier[0] != '_')
    {
        return false;
    }

    // 检查剩余字符是否为字母、数字或下划线
    for (size_t i = 1; i < identifier.size(); ++i)
    {
        if (!std::isalnum(identifier[i]) && identifier[i] != '_')
        {
            return false;
        }
    }

    // 检查是否为C++关键字
    if ((logicKeywords.find(identifier) != logicKeywords.end()) && (functionKeywords.find(identifier) != functionKeywords.end()))
    {
        return false;
    }

    return true;
}

// 处理未知类型的指令
void Interpreter::AddTheValueToThePool(const std::string &instruction)
{
    if (!isValidCppIdentifier(instruction))
    {
        reportError("Invalid variable name", line);
        throw std::runtime_error("Invalid variable name");
    }
    if (instruction == "1" || instruction == "0")
    {
        reportError("'1' or '0' is polluting the variable pool", line);
        throw std::runtime_error("'1' or '0' is polluting the variable pool");
    }
    // 假设未知类型的指令是变量名，并将其添加到变量池中，初始值为false
    if (variablePool.find(instruction) == variablePool.end())
    {
        variablePool[instruction] = false;
        // std::cout << "\033[41mAdded variable: " << instruction << " with initial value false at line " << line << "\033[0m" << std::endl;
    }
}

// 将字符串转换为布尔值
bool Interpreter::convertToBool(const std::string &str)
{
    if (str == "1")
    {
        return true;
    }
    else if (str == "0")
    {
        return false;
    }
    else
    {
        return convertToBool(std::to_string(variablePool[str]));
    }
}

void Interpreter::reportError(const std::string &errorMessage, int line) const
{
    std::cerr << "\033[41m[" << filename << ":" << line << "]Error: " << errorMessage << "\033[0m" << std::endl;
}
