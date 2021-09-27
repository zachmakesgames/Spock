// SpockDemo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../SpockLib/spock.h"

int main()
{
    c_instance test("Test instance");
    c_device test_device(&test);
    c_view test_view(&test, &test_device, 200, 200);

    std::cout << "Spock test\n";
    std::string instr;
    std::cin >> instr;
}

