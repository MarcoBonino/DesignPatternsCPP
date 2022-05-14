#include "Vector.h"

#include <iostream>
#include <string>

struct SomeData
{
    std::string m_string;
    unsigned long m_number;
};

int main()
{
    Vector<int> vInt;
    vInt.pushBack(1);
    vInt.pushBack(2);
    vInt.pushBack(3);
    for (auto elem : vInt) std::cout << elem << " ";
    std::cout << std::endl;
    for (auto& elem : vInt) elem += 30;
    for (const auto& elem : vInt) std::cout << elem << " ";
    std::cout << std::endl;

    Vector<int> vInt2{10,20,30,44,3,6,2,6};
    for (const auto& elem : vInt2) std::cout << elem << " ";
    std::cout << std::endl;

    Vector<SomeData> vecData{{"myString", 44}, {"second string", 200}, {"last", 10}};
    for (const auto& elem : vecData) std::cout << elem.m_string << " " << elem.m_number << " ; ";
    std::cout << std::endl;

    return 0;
}