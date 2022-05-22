#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

class CPUCore
{
public:
    CPUCore(const std::string& isa_, unsigned freqMHz_)
        : isa(isa_)
        , freqMHz(freqMHz_)
    {
        std::cout << "Complex (and time consuming) operation to build a CPUCore...\n";
    }

    virtual ~CPUCore() = default;

    void setFrequency(unsigned freqMHz_) { freqMHz = freqMHz_; }
    unsigned getFrequency() const { return freqMHz; }

    virtual std::string describe() const
    {
        std::stringstream ss;
        ss << "ISA: " << isa << " core running at " << freqMHz << " MHz";
        return ss.str();
    }

    virtual std::unique_ptr<CPUCore> clone() const = 0;

protected:
    std::string isa;
    unsigned freqMHz;
 };

class CPUCore64bit : public CPUCore
{
public:
    // Supposed to be very expensive operation
    CPUCore64bit(const std::string& isa_, unsigned freqMHz_, bool support32bit_)
        : CPUCore(isa_, freqMHz_)
        , support32bit(support32bit_)
    {
        std::cout << "Much more effort to make it 64 bit...\n";
    }

    // Fast: just copy all the values without any computation
    CPUCore64bit(const CPUCore64bit& other)
        : CPUCore(other)
        , support32bit(other.support32bit)
    {
        std::cout << "CPUCore64bit copy-constructor!" << std::endl;
    }

    std::string describe() const override
    {
        std::string regWidth;
        regWidth = support32bit ? " (64/32 bit)" : " (64 bit only)";
        return CPUCore::describe() + regWidth;
    }

    std::unique_ptr<CPUCore> clone() const override
    {
        return std::make_unique<CPUCore64bit>(*this);
    }

protected:
    bool support32bit;
};


int main()
{
    std::cout << "Let's make a prototype 64 bit only core!\n";
    const auto prototype64only = std::make_unique<CPUCore64bit>("ARM AArch64", 0, false);
    std::cout << "Let's make a prototype 64/32 bit core!\n";
    const auto prototype32support = std::make_unique<CPUCore64bit>("ARM AArch64", 0, true);

    std::vector<std::unique_ptr<CPUCore>> cluster;

    auto fastCore = prototype64only->clone();
    fastCore->setFrequency(4000);
    cluster.push_back(std::move(fastCore));
    cluster.emplace_back(cluster[0]->clone());

    auto middleCore = prototype64only->clone();
    middleCore->setFrequency(2800);
    cluster.push_back(std::move(middleCore));
    cluster.emplace_back(cluster[2]->clone());
    cluster.emplace_back(cluster[2]->clone());
    cluster.emplace_back(cluster[2]->clone());

    auto powerEfficientCore = prototype32support->clone();
    powerEfficientCore->setFrequency(1500);
    cluster.push_back(std::move(powerEfficientCore));
    cluster.emplace_back(cluster[6]->clone());
    cluster.emplace_back(cluster[6]->clone());
    cluster.emplace_back(cluster[6]->clone());

    for (const auto& core : cluster)
        std::cout << core->describe() << std::endl;
}
