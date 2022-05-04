#include <iostream>
#include <string>

enum class STATUS : bool
{
    DISCONNECTED = false,
    CONNECTED = true
};

/* The Target */
struct HomeSocketInterface
{
    // Higher than 255 Volts is dangerous at home...
    virtual uint8_t voltage() const = 0;
    virtual STATUS phase() const = 0;
    virtual STATUS neutral() const = 0;
    virtual void deliverPower() const = 0;
};

/* This class is what the client would like to have, but suppose we don't have it! */
class EuropeanHomeSocket : public HomeSocketInterface
{
public:
    EuropeanHomeSocket(STATUS connected_, STATUS eart_status)
        : connected(connected_)
        , eart_connected(eart_status)
    {}

    uint8_t voltage() const override { return 230; }
    STATUS phase() const override { return connected; }
    STATUS neutral() const override { return connected; }
    STATUS earth() const { return eart_connected; }
    void deliverPower() const override { std::cout << "Getting power from home socket..." << std::endl; }

private:
    STATUS connected;
    STATUS eart_connected;
};

/* The Adaptee interface */
struct IndustrialSocketInterface
{
    // Up to several thousands of Volts...
    // Seems not working: https://stackoverflow.com/questions/13463048/c-how-to-multiple-inherits-from-interfaces-with-different-return-types
    //virtual uint16_t voltage() = 0; 
    virtual uint16_t volt() const = 0;
    virtual STATUS phase1() const = 0;
    virtual STATUS phase2() const = 0;
    virtual STATUS phase3() const = 0;
    virtual STATUS neutral() const = 0;
    virtual STATUS earth() const = 0;
    virtual void deliverPower() const = 0;
};

/* The Adaptee */
class EuropeanIndustrialeSocket : public IndustrialSocketInterface
{
public:
    EuropeanIndustrialeSocket(STATUS connected_, STATUS eart_status, uint16_t voltage, bool triphase)
        : connected_monophase(connected_)
        , connected_triphase(triphase ? STATUS::CONNECTED : STATUS::DISCONNECTED)
        , eart_connected(eart_status)
        , V(voltage)
    {}

    uint16_t volt() const override { return V; }
    STATUS phase1() const override { return (connected_triphase == STATUS::CONNECTED) ? STATUS::CONNECTED : connected_monophase; }
    STATUS phase2() const override { return connected_triphase; }
    STATUS phase3() const override { return connected_triphase; }
    STATUS neutral() const override { return (connected_triphase == STATUS::CONNECTED) ? STATUS::CONNECTED : connected_monophase; }
    STATUS earth() const override { return eart_connected; }
    void deliverPower() const override { std::cout << "Getting power from industrial socket (up to 50 KW)..." << std::endl; }

private:
    STATUS connected_monophase;
    STATUS connected_triphase;
    STATUS eart_connected;
    const uint16_t V;
};

/* The Adapter */
class Adapter : public HomeSocketInterface, private EuropeanIndustrialeSocket
{
public:
    Adapter(STATUS connected_)
        : EuropeanIndustrialeSocket(connected_, STATUS::DISCONNECTED, 230, false)
    {}

    uint8_t voltage() const override { return static_cast<uint8_t>(EuropeanIndustrialeSocket::volt()); }
    STATUS phase() const override { return EuropeanIndustrialeSocket::phase1(); }
    STATUS neutral() const override { return EuropeanIndustrialeSocket::neutral(); }
    void deliverPower() const override     
    { 
        EuropeanIndustrialeSocket::deliverPower();
        std::cout << "Adpter limits power to maximun 2300 Watt." << std::endl;
    }
};

struct Phone
{
    void charge()
    {
        std::cout << "Phone is charging..." << std::endl;
    }
};
/* Client */
class PhoneCharger
{
public:
    void plugIn(HomeSocketInterface* supply)
    {
        if (supply->voltage() < 100) // too low
            return;

        if (supply->voltage() > 240)
        {
            std::cout << "BOOM!!! Charger exploded!" << std::endl;
            broken = true;
            return;
        }
        power = supply;
        charge();
    }

    void plugPhone(Phone* p)
    {
        phone = p;
        charge();
    }

private:
    void charge()
    {
        if (!broken)
        {
            if (    phone && power
                && (power->phase() == STATUS::CONNECTED)
                && (power->neutral() == STATUS::CONNECTED))
            {
                power->deliverPower();
                phone->charge();
            }
        }
    }

    HomeSocketInterface* power {nullptr};
    Phone* phone {nullptr};
    bool broken {false};
};


int main()
{
    EuropeanHomeSocket homeSocket(STATUS::CONNECTED, STATUS::CONNECTED);
    Phone phone;

    std::cout << "Charge from home:" << std::endl;
    PhoneCharger* charger = new PhoneCharger();    
    charger->plugIn(&homeSocket);
    charger->plugPhone(&phone);

    std::cout << "\nCharge from the fab:" << std::endl;
    EuropeanIndustrialeSocket industrialSocket(STATUS::CONNECTED, STATUS::CONNECTED, 380, true);
    //charger->plugIn(&industrialSocket); // do not compile: need an adapter!
    Adapter adpter(STATUS::CONNECTED);
    charger->plugIn(&adpter);

    return 0;
}