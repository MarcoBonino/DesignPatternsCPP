#include <array>
#include <cstring>
#include <memory>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>

typedef uint32_t Address;
enum class ACCESS_TYPE : bool
{
    WRITE = false,
    READ = true
};

class BusHandler
{
public:
    BusHandler() = default;
    virtual ~BusHandler() = default;

    BusHandler* setNext(BusHandler* n)
    {
        next = n;
        return n;
    }

    virtual bool handleRequest(Address address, ACCESS_TYPE type, uint32_t& data)
    {
        if (next)
            return next->handleRequest(address, type, data);

        if (verbose) std::cout << "No device was able to handle the request!" << std::endl;
        return false;
    }

private:
    BusHandler* next {nullptr};
    static constexpr bool verbose = true;
};

class MainMemoryBusHandler : public BusHandler
{
public:
    static constexpr size_t RAM_SIZE = 32 * 1024 * 1024; // 32 MB

    MainMemoryBusHandler(Address start_)
    : base(start_)
    {}

    ~MainMemoryBusHandler() = default;

    bool handleRequest(Address address, ACCESS_TYPE type, uint32_t& data) override
    {
        if ((address >= base) && (address < base + RAM_SIZE - sizeof(data)))
        {
            if (type == ACCESS_TYPE::READ)
                memcpy(&data, &memory[address - base], sizeof(data));
            else
                memcpy(&memory[address - base], &data, sizeof(data));
            return true;
        }
        return BusHandler::handleRequest(address, type, data);
    }
private:
    const Address base;
    std::array<uint8_t, RAM_SIZE> memory;
};

class ROMBusHandler : public BusHandler
{
public:
    ROMBusHandler(Address base_, size_t size_) : base(base_), size(size_) {}
    ~ROMBusHandler() = default;

    bool handleRequest(Address address, ACCESS_TYPE type, uint32_t& data) override
    {
        if ((address >= base) && (address < base + size - sizeof(data)))
        {
            if (type == ACCESS_TYPE::READ)
            {
                // actual memory access
                data = 0xDA7ADA7A;
                return true;
            }
            return false; // no write allowed!
        }
        return BusHandler::handleRequest(address, type, data);
    }

private:
    const Address base;
    const size_t size;
};

class FlashMemoryBusHandler : public BusHandler
{
public:
    // Flash memory address is hardcoded
    static constexpr Address START_ADDRESS = 0x4000;
    static constexpr Address END_ADDRESS = 0x800000;
    FlashMemoryBusHandler() = default;
    ~FlashMemoryBusHandler() = default;

    bool handleRequest(Address address, ACCESS_TYPE type, uint32_t& data) override
    {
        if ((address >= START_ADDRESS) && (address < END_ADDRESS - sizeof(data)))
        {
            if (address % sizeof(data))
                return false; // address must be aligned
            else
            {
                if (type == ACCESS_TYPE::WRITE)
                    memory[address] = data;
                else
                {
                    if (memory.contains(address))
                        data = memory[address];
                    else
                        data = 0; // memory has been zeroed
                }
                return true;
            }
        }
        return BusHandler::handleRequest(address, type, data);
    }

private:
    std::unordered_map<Address, uint32_t> memory;
};


int main(int argc, const char* argv[])
{
    auto ram = std::make_unique<MainMemoryBusHandler>(0x1000000);
    auto rom = std::make_unique<ROMBusHandler>(0x3000000, 16 * 1024); // 16 KB ROM
    auto flash = std::make_unique<FlashMemoryBusHandler>();

    auto bus = std::make_unique<BusHandler>();
    bus->setNext(ram.get())->setNext(rom.get())->setNext(flash.get());

    std::vector<std::tuple<Address, ACCESS_TYPE, uint32_t>> accesses
    {
        {0x120, ACCESS_TYPE::WRITE, 0},

        {0x1000020, ACCESS_TYPE::WRITE, 0x12345678},
        {0x1000020, ACCESS_TYPE::READ, 0 /*dummy*/},
        {0x1000022, ACCESS_TYPE::READ, 0 /*dummy*/},

        {0x3000000, ACCESS_TYPE::READ, 0 /*dummy*/},
        {0x3000000, ACCESS_TYPE::WRITE, 0xABCD},
        {0x3000008, ACCESS_TYPE::READ, 0 /*dummy*/},

        {0x6000, ACCESS_TYPE::READ, 0 /*dummy*/},
        {0x6002, ACCESS_TYPE::READ, 0 /*dummy*/},
        {0x71F0, ACCESS_TYPE::WRITE, 0xABCDEF},
        {0x71F4, ACCESS_TYPE::WRITE, 0x1234},
        {0x71F4, ACCESS_TYPE::READ, 0 /*dummy*/},
        {0x71F0, ACCESS_TYPE::READ, 0 /*dummy*/}
    };

    for (const auto& acc : accesses)
    {
        auto [addr, type, data] = acc;
        bool success = bus->handleRequest(addr, type, data);
        if (success)
        {
            std::cout << "Success! ";
            if (type == ACCESS_TYPE::READ)
                std::cout << "Read 0x" << std::hex << data << " at address 0x" << std::hex << addr << std::endl;
            else
                std::cout << "Write 0x" << std::hex << data << " at address 0x" << std::hex << addr << std::endl;
        }
        else
        {
            std::cout << "Failed!" << std::endl;
        }
    }
}
