#include "main.h"

int main(int argc, char **argv)
{
    std::string filename(argv[1]);
    Interpreter interpreter(filename);
    interpreter.Run();

    auto duration_ns = std::stoll(interpreter.getRunDuration());
    if (duration_ns < 1000)
    {
        std::cout << "\033[44mProgram run duration: " << duration_ns << " ns\033[0m" << std::endl;
    }
    else if (duration_ns < 1000000)
    {
        std::cout << "\033[44mProgram run duration: " << duration_ns / 1000.0 << " µs\033[0m" << std::endl;
    }
    else if (duration_ns < 1000000000)
    {
        std::cout << "\033[44mProgram run duration: " << duration_ns / 1000000.0 << " ms\033[0m" << std::endl;
    }
    else
    {
        std::cout << "\033[44mProgram run duration: " << duration_ns / 1000000000.0 << " s\033[0m" << std::endl;
    }
    return 0;
}