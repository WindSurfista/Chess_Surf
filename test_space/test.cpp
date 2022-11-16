#include <iostream>
#include "../attacks.hpp"
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


int main()
{
    thing foo;
    foo.func(10);
    thing bar(foo);

    bar.func(2);
    foo = bar;
    foo.dis();
    bar.dis();
}
