// SpockDemo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../SpockLib/spock.h"

int main()
{
    Instance test("Test instance");
    Device testDevice(&test);
    View testView(&test, &testDevice, 200, 200);

    std::cout << "Spock test\n";
    std::string instr;
    std::cin >> instr;
}

