#include <iostream>
#include "DynamicDecorator.h"
#include "StaticDecorator.h"

void dynamicDecoratorDemo()
{
    using namespace DynamicDecorator;
    std::cout << "\n*********** Dynamic Decorator Demo ***********" << std::endl;

    SimpleIceCream gelato("chocolate");
    std::cout << gelato.describe() << std::endl;

    IceCreamWithTopping gelatoPlus(gelato, "panna");
    std::cout << gelatoPlus.describe() << std::endl;

    IceCreamInCone gelatoPlusCone(gelato, "wafer");
    std::cout << gelatoPlusCone.describe() << std::endl;

    IceCreamInCone gelatoPlusPlus(gelatoPlus, "cialda");
    std::cout << gelatoPlusPlus.describe() << std::endl;

    MultiBowlIceCream gelato3bowls({"pistachio", "chocolate", "hazelnut"});
    std::cout << gelato3bowls.describe() << std::endl;

    IceCreamWithTopping gelatoMax(gelato3bowls, "chantilly cream");
    IceCreamInCone gelatoSuper(gelatoMax, "chocolate-galssed cialda");
    std::cout << gelatoSuper.describe() << std::endl;
    gelato3bowls.eatOneBowl();
    //gelatoSuper.eatOneBowl(); // cannot call it from a decorated ice-cream
}

void staticDecoratorDemo()
{
    using namespace StaticDecorator;
    std::cout << "\n*********** Static Decorator Demo ***********" << std::endl;

    IceCreamWithTopping<MultiBowlIceCream> toppedGelato("chopped hazelnuts", std::initializer_list<std::string>{"vanilla", "chocolate", "blueberry", "hazelnut"});
    std::cout << toppedGelato.describe() << std::endl;

    toppedGelato.eatOneBowl();
    std::cout << toppedGelato.describe() << std::endl;
    toppedGelato.eatOneBowl();
    std::cout << toppedGelato.describe() << std::endl;

    IceCreamInCone<IceCreamWithTopping<MultiBowlIceCream>> bigGelato("big chocolate wafer", "chantilly cream",
        std::initializer_list<std::string>{"chocolate", "strawberry", "lemon", "panna cotta"});
    std::cout << bigGelato.describe() << std::endl;
}

int main()
{
    dynamicDecoratorDemo();
    staticDecoratorDemo();
}
