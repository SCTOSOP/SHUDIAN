#include "main.h"

int main(int agc, char **argv)
{
    File file(argv[1]);
    std::string content = file.readFile();
    std::vector<std::string> words = file.splitContent(content);
    Interpreter interpreter(words);
    interpreter.Run();
    return 0;
}