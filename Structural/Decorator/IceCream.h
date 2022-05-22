#pragma once

#include <vector>

struct IceCream
{
    virtual std::string describe() const = 0;
    virtual ~IceCream() = default;
};

/* One IceCream concrete class */
class SimpleIceCream : public IceCream
{
public:
    explicit SimpleIceCream(const std::string bowl_)
        : bowl(bowl_)
    {}

    std::string describe() const override
    {
        return { "Ice-cream with " + bowl + " bowl" };
    }

private:
    const std::string bowl;
};

/* Another IceCream concrete class */
class MultiBowlIceCream : public IceCream
{
public:
    explicit MultiBowlIceCream(std::initializer_list<std::string> bowls_)
        : bowls(bowls_)
    {}

    std::string describe() const override
    {
        std::string out("Ice-cream with");
        if (!bowls.empty())
        {
            for (const auto& bowl : bowls)
            {
                out.push_back(' ');
                out.append(bowl);
            }
        }
        else
        {
            out.append(" no bowls ");
        }
        return out;
    }

    void eatOneBowl()
    {
        if (!bowls.empty())
            bowls.pop_back();
    }

private:
    std::vector<std::string> bowls;
};
