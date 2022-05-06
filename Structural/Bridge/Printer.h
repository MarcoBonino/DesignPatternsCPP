#pragma once

#include <cstdint>

class Printer
{
    class PrinterImpl;
public:
    Printer();

    void printBigNumber(uint64_t number);
    void printNumber(uint32_t number);
    void printNumberAsChar(uint8_t number);

private:
    PrinterImpl* implementaion; // The Pimpl
};
