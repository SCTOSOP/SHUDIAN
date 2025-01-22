#include "interpreter.h"
#include "file.h"
#include <iostream>

// 初始化关键字集合
const std::unordered_set<std::string> Interpreter::logicKeywords = {"and", "or", "not", "min", "set"};
const std::unordered_set<std::string> Interpreter::functionKeywords = {"print", "input"};

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
		line++;
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

	size_t currentIndexInLine = 0;
	if (currentLine[0] == "input")
	{
		auto pause_time = std::chrono::high_resolution_clock::now();
		currentIndexInLine++;
		while (currentIndexInLine < currentLine.size())
		{
			if (!isValidCppIdentifier(currentLine[currentIndexInLine]))
			{
				reportError("Invalid identifier", line);
				throw std::runtime_error("Invalid identifier");
			}
			std::cout << "\033[44m请输入变量" << currentLine[currentIndexInLine] << ":\033[0m ";
			std::string input;
			std::cin >> input;
			this->variablePool[currentLine[currentIndexInLine]] = std::make_shared<bool>(convertToBool(input));
			currentIndexInLine++;
		}
		auto resume_time = std::chrono::high_resolution_clock::now();
		start_time_ += (resume_time - pause_time);

		return;
	}
	else if (currentLine[0] == "print")
	{
		currentIndexInLine++;
		while (currentIndexInLine < currentLine.size())
		{
			std::shared_ptr<bool> tmp_bool;
			if (variablePool.find(currentLine[currentIndexInLine]) != variablePool.end())
			{
				tmp_bool = variablePool[currentLine[currentIndexInLine]];
			}
			else if (LogicPool.find(currentLine[currentIndexInLine]) != LogicPool.end())
			{
				auto &logic = LogicPool[currentLine[currentIndexInLine]];
				tmp_bool = std::make_shared<bool>(logic.isValueInMins());
			}
			else
			{
				reportError("Variable not found", line);
				throw std::runtime_error("Variable not found");
			}
			std::cout << "\033[42m" << currentLine[currentIndexInLine] << " = " << *tmp_bool << "\033[0m" << std::endl;
			currentIndexInLine++;
		}

		return;
	}
	if (currentLine.size() < 2)
	{
		reportWarning("Unused line", line);
		return;
	}

	bool tmp_logic_bool = false;

	if (currentLine[1] == "and" || currentLine[1] == "or" || currentLine[1] == "not")
	{
		auto subLine = std::vector<std::string>(currentLine.begin() + 1, currentLine.end());
		auto words = this->GetLogicWords(subLine);
		tmp_logic_bool = this->LogicProcessing(words);
		this->variablePool[currentLine[0]] = std::make_shared<bool>(tmp_logic_bool);

		return;
	}

	if (currentLine[1] == "set")
	{
		Logic logic;
		for (size_t i = 2; i < currentLine.size(); i++)
		{
			if (variablePool.find(currentLine[i]) == variablePool.end())
			{
				reportError("Variable not found", line);
				throw std::runtime_error("Variable not found");
			}
			logic.values.push_back(this->variablePool[currentLine[i]]);
		}
		this->LogicPool[currentLine[0]] = logic;

		return;
	}
	else if (currentLine[1] == "min")
	{
		if (LogicPool.find(currentLine[0]) == LogicPool.end())
		{
			reportError("Variable not found", line);
			throw std::runtime_error("Variable not found");
		}
		auto &logic = LogicPool[currentLine[0]];
		for (size_t i = 2; i < currentLine.size(); i++)
		{
			int tmp_int = atoi(currentLine[i].c_str());
			if (tmp_int < 0 || tmp_int > pow(2, logic.values.size()) - 1)
			{
				reportWarning("Meaningless number", line);
			}
			else
			{
				logic.mins.insert(tmp_int);
			}
		}

		return;
	}

	if (currentLine[1] == "1" || currentLine[1] == "0")
	{
		variablePool[currentLine[0]] = std::make_shared<bool>(convertToBool(currentLine[1]));
	}
	else if (variablePool.find(currentLine[1]) != variablePool.end())
	{
		variablePool[currentLine[0]] = variablePool[currentLine[1]];
	}
	else if (LogicPool.find(currentLine[1]) != LogicPool.end())
	{
		auto &logic = LogicPool[currentLine[1]];
		variablePool[currentLine[0]] = std::make_shared<bool>(logic.isValueInMins());
	}
	else
	{
		reportError("Variable not found", line);
		throw std::runtime_error("Variable not found");
	}
}

std::string Interpreter::getRunDuration() const
{
	return run_duration_;
}

std::vector<std::vector<std::string>> Interpreter::GetLogicWords(std::vector<std::string> &currentLine)
{
	size_t currentIndexInWords = 0;
	std::vector<std::vector<std::string>> words;
	std::vector<std::string> value;
	size_t NumberOfLayers = 0;
	while (currentIndexInWords < currentLine.size())
	{
		if (currentLine[currentIndexInWords] == "[")
		{
			NumberOfLayers++;
			value.push_back(currentLine[currentIndexInWords]);
			currentIndexInWords++;
		}
		else if (currentLine[currentIndexInWords] == "]")
		{
			NumberOfLayers--;
			value.push_back(currentLine[currentIndexInWords]);
			currentIndexInWords++;
			if (NumberOfLayers == 0)
			{
				words.push_back(value);
				value.clear();
			}
		}
		else
		{
			value.push_back(currentLine[currentIndexInWords]);
			currentIndexInWords++;
			if (NumberOfLayers == 0)
			{
				words.push_back(value);
				value.clear();
			}
		}
	}

	return words;
}

bool Interpreter::LogicProcessing(std::vector<std::vector<std::string>> &words)
{
	LogicKeywords lgs = LogicKeywords::none;
	bool re = false;
	if (words[0][0] == "and")
	{
		lgs = LogicKeywords::AND;
		re = true;
	}
	else if (words[0][0] == "or")
	{
		lgs = LogicKeywords::OR;
		re = false;
	}
	else if (words[0][0] == "not")
	{
		lgs = LogicKeywords::NOT;
		re = false;
	}

	for (auto word = words.begin() + 1; word != words.end(); word++)
	{
		bool tmp;
		if ((*word)[0] != "[")
		{
			if (this->variablePool.find((*word)[0]) == this->variablePool.end())
			{
				reportError("Variable not found", line);
				throw std::runtime_error("Variable not found");
			}
			else
			{
				tmp = *variablePool[(*word)[0]];
			}
		}
		else
		{
			auto subword = std::vector<std::string>(word->begin() + 1, word->end() - 1);
			auto words_next = this->GetLogicWords(subword);
			tmp = this->LogicProcessing(words_next);
		}

		switch (lgs)
		{
		case LogicKeywords::AND:
			re = re & tmp;
			break;
		case LogicKeywords::OR:
			re = re | tmp;
			break;
		case LogicKeywords::NOT:
			return !(tmp);
		}
	}

	return re;
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

	// 检查是否为关键字
	if ((logicKeywords.find(identifier) != logicKeywords.end()) && (functionKeywords.find(identifier) != functionKeywords.end()))
	{
		return false;
	}

	return true;
}

// 将字符串转换为布尔值
bool Interpreter::convertToBool(const std::string &str)
{
	if (str == "1")
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Interpreter::reportError(const std::string &errorMessage, int line) const
{
	std::cerr << "\033[41m[" << filename << ":" << line << "]Error: " << errorMessage << "\033[0m" << std::endl;
}

void Interpreter::reportWarning(const std::string &warningMessage, int line) const
{
	std::cerr << "\033[43m[" << filename << ":" << line << "]Warning: " << warningMessage << "\033[0m" << std::endl;
}

bool Logic::isValueInMins() const
{
	int decimalValue = 0;
	for (size_t i = 0; i < values.size(); ++i)
	{
		if (*values[i])
		{
			decimalValue += (1 << (values.size() - 1 - i));
		}
	}
	return mins.find(decimalValue) != mins.end();
}
