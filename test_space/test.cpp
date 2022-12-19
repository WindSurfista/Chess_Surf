#include <iostream>
#include <string>
#include <math.h>

void pseudo_random()
{
    double value = 5;
    while (true)
    {
        value = std::pow((1 / std::tan(value)),2);
        std::cout << (value - ((int)value))  << std::endl;
    }
}

class thing
{
    public:
    int x = 5;

    void func(int num) {x=num;}
    void dis() {std::cout << x << std::endl;}
};

int main(int argc, char** argv)
{
    std::cout << "You have entered " << argc << " arguments:" << "\n";
    for (int i = 0; i < argc; ++i)
    std::cout << argv[i] << "\n";
    return 0;
}