#include <array>
#include <iostream>
#include <optional>
#include <string>

class DesktopComputer
{
public:
    DesktopComputer() {}

    DesktopComputer(DesktopComputer&& other) = default;

    enum class CoolingType : uint8_t
    {
        passive
       ,fan
       ,liquid
    };

    static constexpr size_t PCIE_SLOTS = 5;

    bool addPcieDevice(const std::string& deviceName, size_t slotNumber)
    {
        if (slotNumber < PCIE_SLOTS)
        {
            if (pcieSlots[slotNumber].has_value())
            {
                std::cout << "Slot number " << slotNumber << " is already used." << std::endl;
                return false; // failed to add device
            }
            else
            {
                pcieSlots[slotNumber] = deviceName;
                return true; // device added
            }
        }
        std::cout << "Invalid slot number (" << slotNumber << ")." << std::endl;
        return false; // failed to add device
    }

    void removePcieDevice(const std::string& deviceName, size_t slotNumber)
    {
        if (slotNumber < PCIE_SLOTS)
        {
            pcieSlots[slotNumber].reset();
        }
    }

    void printSpecs()
    {
        std::string coolingString;
        switch (cooling)
        {
            case CoolingType::passive:
                coolingString = "passive";
                break;
            case CoolingType::fan:
                coolingString = "fan";
                break;
            case CoolingType::liquid:
                coolingString = "liquid";
                break;
            default:
                break;
        }

        auto threadCount = hyperThreading ? cpuCoreCount * 2 : cpuCoreCount;

        std::cout << "******************************\n"
        "Case model: " << caseModel << "\n"
        "Cooling type: " << coolingString << "\n"
        "CPU cores: " << cpuCoreCount << "(" << threadCount << " threads)\n"
        "CPU frequency: " << cpuFreqInMHz << " MHz\n"
        "RAM size: " << ramSizeInGB << " GB\n";

        std::cout << "PCI-E slots:" << std::endl;
        for (const auto& pcie : pcieSlots)
            std::cout << "  " << pcie.value_or("Empty slot") << std::endl;
        std::cout << "******************************" << std::endl;
    }

private:
    std::string caseModel;
    CoolingType cooling;

    uint32_t ramSizeInGB {1};
    uint32_t cpuCoreCount {1};
    bool hyperThreading {false};
    uint32_t cpuFreqInMHz {1000};
    std::array<std::optional<std::string>, PCIE_SLOTS> pcieSlots;

    friend class DesktopComputerBuilder;
};

class DesktopComputerBuilder
{
public:
    DesktopComputerBuilder() {}

    operator DesktopComputer()
    {
        return std::move(PC);
    }

    DesktopComputerBuilder& withCase(const std::string& caseModel)
    {
        PC.caseModel = caseModel;
        return *this;
    }

    DesktopComputerBuilder& coolUsing(const DesktopComputer::CoolingType cooling)
    {
        PC.cooling = cooling;
        return *this;
    }

    DesktopComputerBuilder& withMemorySize(uint32_t sizeGB)
    {
        PC.ramSizeInGB = sizeGB;
        return *this;
    }

    DesktopComputerBuilder& withCoreCount(uint32_t n)
    {
        PC.cpuCoreCount = n;
        return *this;
    }

    DesktopComputerBuilder& withHyperThreadingCPU()
    {
        PC.hyperThreading = true;
        return *this;
    }

    DesktopComputerBuilder& withCPUFreq(uint32_t freqMHz)
    {
        PC.cpuFreqInMHz = freqMHz;
        return *this;
    }

    DesktopComputerBuilder& withPciE(const std::string& deviceName, size_t slotNumber)
    {
        PC.addPcieDevice(deviceName, slotNumber);
        return *this;
    }

private:
    DesktopComputer PC;
};


int main(int argc, const char * argv[])
{
    using enum DesktopComputer::CoolingType;
    DesktopComputer myPC = DesktopComputerBuilder()
        .withCase("Thermaltake white case").coolUsing(liquid)
        .withCoreCount(10).withCPUFreq(4000).withHyperThreadingCPU()
        .withMemorySize(128)
        .withPciE("GPU card 1", 0).withPciE("GPU card 2", 2).withPciE("Wifi extension card", 4);

    myPC.printSpecs();
    return 0;
}
