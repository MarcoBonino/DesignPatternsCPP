#include "Printer.h"

#include <iostream>
#include <type_traits>

template <typename T>
concept isNumber = std::is_arithmetic_v<T>;

struct Printer::PrinterImpl
{
    template <typename T>
    requires isNumber<T>
    void printNumber(T number)
    {
        std::cout << std::dec << number << std::endl;
    }

    void print_uint64(uint64_t x)
    {
        // Some complex operations...
        std::cout << "0x" << std::hex << x << std::endl;
    }
};


Printer::Printer()
    : implementaion(new PrinterImpl)
{}

void Printer::printBigNumber(uint64_t number)
{
    implementaion->print_uint64(number);
}

void Printer::printNumber(uint32_t number)
{
    implementaion->printNumber(number);
}

void Printer::printNumberAsChar(uint8_t number)
{
    implementaion->printNumber(number);
}
