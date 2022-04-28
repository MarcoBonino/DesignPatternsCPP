
#include <iostream>
#include <memory>
#include <string>
#include <stack>

class Player
{
public:
    Player(const std::string& name)
    : name(name)
    {}

    void undo()
    {
        if (changes.empty())
        {
            std::cout << "Warning! Nothing to undo." << std::endl;
            return;
        }
        // save current state in case we later want to do redo()
        undoneChanges.emplace(std::make_shared<const Memento>(healt, attackPower, level));
        auto m = changes.top();
        restoreFromMemento(*m);
        changes.pop();
    }

    void redo()
    {
        if (undoneChanges.empty())
        {
            std::cout << "Warning! Nothing to redo." << std::endl;
            return;
        }
        // save current state in case we later want to do undo()
        changes.emplace(std::make_shared<const Memento>(healt, attackPower, level));
        auto m = undoneChanges.top();
        restoreFromMemento(*m);
        undoneChanges.pop();
    }

    void workout()
    {
        saveMemento();
        if (attackPower < POWER_MAX)
            attackPower++;
    }

    void takeMedicine()
    {
        saveMemento();
        if (healt < HEALT_MAX)
            healt++;
    }

    void fight(uint8_t enemyPower)
    {
        saveMemento();
        constexpr uint8_t penality = 5;
        if (attackPower > enemyPower)
        {
            if (level < LEVEL_MAX)
                level++;
        }
        else
        {
            if (level > LEVEL_START)
                level--;
            if (healt > penality)
                healt -= penality;
            else
                healt = 0;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Player& player)
    {
        return os << player.name << " (Healt = " << unsigned(player.healt)
        << "%, Power = " << unsigned(player.attackPower)
        << "%) is at level " << unsigned(player.level);
    }

private:
    static constexpr uint8_t HEALT_MAX = 100;
    static constexpr uint8_t POWER_MAX = 100;
    static constexpr uint8_t POWER_START = 10;
    static constexpr uint8_t LEVEL_START = 1;
    static constexpr uint8_t LEVEL_MAX = 15;

    const std::string name;
    uint8_t healt{HEALT_MAX};
    uint8_t attackPower{POWER_START};
    uint8_t level{LEVEL_START};

    struct Memento final
    {
        Memento(uint8_t healt, uint8_t attackPower, uint8_t level)
        : m_healt(healt), m_attackPower(attackPower), m_level(level)
        {}

        const uint8_t m_healt;
        const uint8_t m_attackPower;
        const uint8_t m_level;
    };

    void saveMemento()
    {
        changes.emplace(std::make_shared<const Memento>(healt, attackPower, level));
        // Invalidate the undoneChanges since redo() operation has no meaning if the previous one was not undo().
        while (!undoneChanges.empty()) undoneChanges.pop();
    }

    void restoreFromMemento(const Memento& m)
    {
        healt = m.m_healt;
        attackPower = m.m_attackPower;
        level = m.m_level;
    }

    std::stack<std::shared_ptr<const Memento>> changes;
    std::stack<std::shared_ptr<const Memento>> undoneChanges;
};


int main(int argc, const char* argv[])
{
    Player p1("Marco");
    std::cout << p1 << std::endl;

    for (auto i = 0; i < 5; i++)
        p1.workout();
    std::cout << p1 << std::endl;

    p1.undo(); std::cout << p1 << std::endl;

    p1.fight(12); std::cout << p1 << std::endl;
    p1.fight(13); std::cout << p1 << std::endl;
    p1.fight(20); std::cout << p1 << std::endl;
    p1.undo(); std::cout << p1 << std::endl;
    p1.fight(19); std::cout << p1 << std::endl;

    p1.redo(); std::cout << p1 << std::endl;

    p1.takeMedicine(); std::cout << p1 << std::endl;
    p1.workout(); std::cout << p1 << std::endl;
    p1.workout(); std::cout << p1 << std::endl;
    p1.undo(); std::cout << p1 << std::endl;
    p1.undo(); std::cout << p1 << std::endl;
    p1.undo(); std::cout << p1 << std::endl;
    p1.redo(); std::cout << p1 << std::endl;
    p1.undo(); std::cout << p1 << std::endl;

    p1.undo(); std::cout << p1 << std::endl;

    return 0;
}
