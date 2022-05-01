#include <iostream>

/* Abstract products */
class CPU
{
public:
    virtual ~CPU() = default;
    virtual void fetch() = 0;
    virtual void execute() = 0;
};

class GPU
{
public:
    virtual ~GPU() = default;
    virtual void renderImage() = 0;
};

/* Concrete products */
class ArmCPU : public CPU
{
public:
    void fetch() override
    {
        std::cout << "ARM fetch instruction.\n";
    }
    void execute() override
    {
        std::cout << "ARM execute instruction.\n";
    }
};

class IntelCPU : public CPU
{
public:
    void fetch() override
    {
        std::cout << "Intel fetch instruction.\n";
    }
    void execute() override
    {
        std::cout << "Intel execute instruction.\n";
    }
};

class ArmGPU : public GPU
{
public:
    void renderImage() override
    {
        std::cout << "ARM MALI: image ready for monitor.\n";
    }
};

class IntelGPU : public GPU
{
public:
    void renderImage() override
    {
        std::cout << "Intel HD graphic: image ready for monitor.\n";
    }
};

/* Abstract Factory */
class AbstractSiliconFactory
{
public:
    virtual ~AbstractSiliconFactory() = default;
    virtual CPU* makeCPU() = 0;
    virtual GPU* makeGPU() = 0;
};

/* Concrete factories */
class ARMSiliconFactory : public AbstractSiliconFactory
{
public:
    CPU* makeCPU()
    {
        return new ArmCPU();
    }
    GPU* makeGPU()
    {
        return new ArmGPU();
    }
};

class IntelSiliconFactory : public AbstractSiliconFactory
{
public:
    CPU* makeCPU()
    {
        return new IntelCPU();
    }
    GPU* makeGPU()
    {
        return new IntelGPU();
    }
};

/* Client code */
class Tester
{
public:
    Tester(AbstractSiliconFactory* factory_)
    : factory(factory_)
    {}

    ~Tester()
    {
        delete factory;
    }

    void testCPU()
    {
        CPU* cpu = factory->makeCPU();
        cpu->fetch();
        cpu->execute();
    }

    void testGPU()
    {
        GPU* gpu = factory->makeGPU();
        gpu->renderImage();
    }

private:
    AbstractSiliconFactory* factory {nullptr};
};

int main()
{
    Tester ArmTester(new ARMSiliconFactory());
    ArmTester.testCPU();
    ArmTester.testGPU();

    Tester IntelTester(new IntelSiliconFactory());
    IntelTester.testCPU();
    IntelTester.testGPU();
}