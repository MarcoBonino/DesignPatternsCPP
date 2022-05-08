#pragma once

#include <string>
#include <type_traits>
#include "IceCream.h"

namespace StaticDecorator
{

template <typename T>
concept isIceCream = std::is_base_of_v<IceCream, T>;

/* Decorator for topping */
template <typename T>
requires isIceCream<T>
class IceCreamWithTopping : public T
{
public:
    template <typename ... ARGS>
    IceCreamWithTopping(const std::string topping_, ARGS ... args)
        : T(std::forward<ARGS>(args)...)
        , topping(topping_)
    {}

    std::string describe() const override
    {
        return { T::describe() + " and " + topping + " on top"};
    }

private:
    std::string topping;
};

/* Decorator for cone */
template <typename T>
requires isIceCream<T>
class IceCreamInCone : public T
{
public:
    template <typename ... ARGS>
    IceCreamInCone(const std::string cone_, ARGS ... args)
        : T(std::forward<ARGS>(args)...)
        , cone(cone_)
    {}

    std::string describe() const override
    {
        return { T::describe() + " served in " + cone + " cone" };
    }

private:
    std::string cone;
};

} // namespace StaticDecorator