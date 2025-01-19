#include "interpreter.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>

// 初始化关键字集合
const std::unordered_set<std::string> Interpreter::logicKeywords = {"and", "or", "not"};
const std::unordered_set<std::string> Interpreter::functionKeywords = {"print", "input"};

// 构造函数，接受std::vector<std::string>作为参数
Interpreter::Interpreter(const std::vector<std::string> &instructions)
    : instructions(instructions), currentInstruction(0), line(1) {}

// 运行解释器
void Interpreter::Run()
{
    while (currentInstruction < instructions.size())
    {
        Step();
    }
}

// 单步执行
void Interpreter::Step()
{
    std::string left_;
    std::vector<std::string> right_;
    FunctionKeywords func = FunctionKeywords::none;
    LogicKeywords lgs = LogicKeywords::none;

    bool loop = true;
    while (currentInstruction < instructions.size() && loop)
    {
        std::string instruction = instructions[currentInstruction];
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
        case InstructionType::Separator:
            // 处理分割符
            line++;       // 遇到分隔符，行号加一
            loop = false; // 结束当前步骤并跳出while循环
            break;
        case InstructionType::Unknown:
            // 处理用户变量
            if (left_.empty())
            {
                handleUnknownInstruction(instruction);
                left_ = instruction;
            }
            else
            {
                if (variablePool.find(instruction) == variablePool.end())
                {
                    std::cerr << "\033[41mError: Undefined variable " << instruction << " used as right value at line " << line << "\033[0m" << std::endl;
                    throw std::runtime_error("Undefined variable used as right value");
                }
                right_.push_back(instruction);
            }
            break;
        case InstructionType::Constant:
            // 处理常量
            if (func == FunctionKeywords::none && left_.empty())
            {
                std::cerr << "\033[41mError: Constant cannot be used as left value at line " << line << "\033[0m" << std::endl;
                throw std::runtime_error("Constant cannot be used as left value");
            }
            if (!right_.empty())
            {
                std::cerr << "\033[41mError: Extra right value at line " << line << "\033[0m" << std::endl;
                throw std::runtime_error("Extra right value");
            }
            right_.push_back(instruction);
            break;
        case InstructionType::Illegal:
            // 处理错误
            std::cerr << "\033[41mError: Illegal instruction encountered at line " << line << "\033[0m" << std::endl;
            throw std::runtime_error("Illegal instruction encountered");
            break;
        }

        currentInstruction++;
    }

    if (right_.empty())
    {
        std::cerr << "\033[41mError: Right value should not be empty in a statement at line " << line << "\033[0m" << std::endl;
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
        std::string tmp_input = "0";
        std::cout << "\033[42m请输入变量 " << left_ << " :\033[0m";
        std::cin >> tmp_input;
        variablePool[left_] = convertToBool(tmp_input);
        break;
    }
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
    if (instruction == ";")
    {
        return InstructionType::Separator;
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

// 处理未知类型的指令
void Interpreter::handleUnknownInstruction(const std::string &instruction)
{
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
