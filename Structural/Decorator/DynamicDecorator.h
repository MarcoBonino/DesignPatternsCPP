#pragma once

#include <string>
#include "IceCream.h"

namespace DynamicDecorator
{

/* Decorator for topping */
class IceCreamWithTopping : public IceCream
{
public:
    IceCreamWithTopping(IceCream& iceCream_, const std::string& topping_)
        : iceCream(iceCream_)
        , topping(topping_)
    {}

    std::string describe() const override
    {
        return { iceCream.describe() + " and " + topping + " on top"};
    }

private:
    IceCream& iceCream;
    std::string topping;
};

/* Decorator for cone */
class IceCreamInCone : public IceCream
{
public:
    IceCreamInCone(IceCream& iceCream_, const std::string& cone_)
        : iceCream(iceCream_)
        , cone(cone_)
    {}

    std::string describe() const override
    {
        return { iceCream.describe() + " served in " + cone + " cone" };
    }

private:
    IceCream& iceCream;
    std::string cone;
};

} // namespace DynamicDecorator
