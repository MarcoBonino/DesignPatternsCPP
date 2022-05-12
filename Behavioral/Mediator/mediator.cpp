#include "IPv4Address.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/* Mediator interface */
struct Router
{
    virtual ~Router() = default;
    virtual void forwardPacket(IPv4Address from, IPv4Address to, const std::vector<uint8_t> data) = 0;
};

/* Object that needs mediator to talk with peers */
class Client
{
public:
    Client(IPv4Address addr) : address(addr) {}

    void sendPacket(IPv4Address to, const std::vector<uint8_t> data)
    {
        if (router)
            router->forwardPacket(address, to, data);
    }

    void receivePacket(IPv4Address from, std::vector<uint8_t> data)
    {
        using namespace std;
        cout << static_cast<string>(address) << " got packet from "
                  << static_cast<string>(from) << "\nData = [";

        for (const auto& d : data)
            cout << " " << setw(2) << setfill('0') << hex  << static_cast<unsigned>(d) << " ";
        cout << "]\n" << std::endl;
    }

    IPv4Address getAddress() const { return address; }
    void setAddress(IPv4Address addr) { address = addr; }
    void connectToRouter(Router* r) { router = r; }

private:
    IPv4Address address;
    Router* router {nullptr};
};

/* Mediator concrete implementation */
struct SimpleRouter : public Router
{
public:
    void forwardPacket(IPv4Address from, IPv4Address to, const std::vector<uint8_t> data) override
    {
        auto destination = network.find(to);
        if (destination != network.end())
        {
            destination->second->receivePacket(from, data);
        }
    }

    void joinNetwork(Client& client)
    {
        IPv4Address address = client.getAddress();
        while (network.contains(address))
        {
            address = firstAvailableAddress++;
        }
        network[address] = &client;
        client.connectToRouter(this);
    }

private:
    std::unordered_map<IPv4Address, Client*> network;
    IPv4Address firstAvailableAddress {1};
};

int main()
{
    SimpleRouter router;

    Client c1(50529027); // 3.3.3.3
    Client c2("10.5.6.3");
    Client c3("5.5.5.5");
    Client c4("205.133.0.2");

    router.joinNetwork(c1);
    router.joinNetwork(c2);
    router.joinNetwork(c3);
    router.joinNetwork(c4);

    c1.sendPacket("5.5.5.5", { 0x1, 0xAB, 0x4F });
    c2.sendPacket("205.133.0.2", { 0x41, 0xFF, 0xFF, 0xFF });
    c3.sendPacket("3.3.3.3", { 0x14, 0x0 });
    c4.sendPacket("10.10.10.10", { 0x12, 0x34 }); // will be discarded: nobody has this IP.
    c4.sendPacket("5.5.5.5", { 0xAB, 0xCD });

    return 0;
}
