#pragma once

#include <concepts>
#include <stdexcept>
#include <string>

struct IPv4Address
{
    IPv4Address(uint32_t addr) : address(addr) {}
    IPv4Address(const char* addr) : IPv4Address(std::string(addr)) {}
    IPv4Address(const std::string& addr)
    {
        address = 0;
        size_t start = 0;
        size_t end = 0;
        for (int i = 3; i >= 0; i--)
        {
            end = addr.find_first_of('.', start);
            std::string byteString = addr.substr(start, end - start);
            if ((byteString.length() > 3) || byteString.empty())
                throw std::runtime_error("Error when oparsing IP address!");
            uint32_t byte = std::stoul(byteString);
            if (byte > 255)
                throw std::runtime_error("Error when oparsing IP address!");
            start = end + 1;
            address |= byte << (8 * i);
        }
        if (end != addr.npos)
            throw std::runtime_error("Error when oparsing IP address!");
    }

    IPv4Address& operator++() { address++; return *this; }
    IPv4Address operator++(int)
    {
        IPv4Address temp = *this;
        address++;
        return temp;
    }

    bool operator==(const IPv4Address& other) { return address == other.address; }

    operator std::string() const
    {
        constexpr uint32_t mask{ 0xFF };
        uint8_t byte0 =  address        & mask;
        uint8_t byte1 = (address >> 8)  & mask;
        uint8_t byte2 = (address >> 16) & mask;
        uint8_t byte3 = (address >> 24) & mask;

        std::string s;
        s.append(std::to_string(byte3));
        s.push_back('.');
        s.append(std::to_string(byte2));
        s.push_back('.');
        s.append(std::to_string(byte1));
        s.push_back('.');
        s.append(std::to_string(byte0));

        return s;
    }

    uint32_t address;
};

bool operator==(const IPv4Address& left, const IPv4Address& right) { return left.address == right.address; }

namespace std
{
    template <>
    struct hash<IPv4Address>
    {
        size_t operator()(const IPv4Address& ipAddress) const
        {
            return hash<uint32_t>()(ipAddress.address);
        }
    };
}

template<typename T>
concept Hashable = requires(T a)
{
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

static_assert(Hashable<IPv4Address>);
