#include <iostream>
#include <string>

class BankAccount
{
    friend class BankAccountCommand;
public:
    BankAccount(const std::string& name_)
    : name(name_)
    {}

    void printStats()
    {
        std::cout << "Account \"" << name << "\" - Balance: " << balance << " EUR" << std::endl;
    }

private:
    void deposit(unsigned amount)
    {
        balance += amount;
    }

    bool withdraw(unsigned amount)
    {
        if (balance >= amount)
        {
            balance -= amount;
            return true;
        }
        return false;
    }

    const std::string name;
    unsigned balance {0};
};

struct Command
{
    virtual void call() = 0;
    virtual void undo() = 0;
};

class BankAccountCommand : Command
{
public:
    enum class Action
    {
        deposit,
        withdraw
    };

    BankAccountCommand(BankAccount& ba, Action action_, unsigned amount_)
    : account(ba)
    , action(action_)
    , amount(amount_)
    {}

    void call() override
    {
        switch (action)
        {
        case Action::deposit:
            account.deposit(amount);
            break;

        case Action::withdraw:
            successfulAction = account.withdraw(amount);
            break;

        default:
            break;
        }
    }

    void undo() override
    {
        if (undoneAction)
            return;

        undoneAction = true;
        switch (action)
        {
        case Action::deposit:
            account.withdraw(amount);
            break;

        case Action::withdraw:
            if (successfulAction)
                account.deposit(amount);
            break;

        default:
            undoneAction = false;
            break;
        }
    }

private:
    BankAccount& account;
    const Action action;
    bool successfulAction {true};
    bool undoneAction {false};
    const unsigned amount;
};


int main(int argc, const char* argv[])
{
    BankAccount myAccount("Saving account");
    myAccount.printStats();

    BankAccountCommand cmd_0{myAccount, BankAccountCommand::Action::deposit, 100};
    BankAccountCommand cmd_1{myAccount, BankAccountCommand::Action::deposit, 30};
    BankAccountCommand cmd_2{myAccount, BankAccountCommand::Action::withdraw, 250};
    BankAccountCommand cmd_3{myAccount, BankAccountCommand::Action::withdraw, 40};

    cmd_0.call();
    myAccount.printStats();

    cmd_1.call();
    myAccount.printStats();

    cmd_2.call();
    myAccount.printStats();

    cmd_1.undo();
    myAccount.printStats();
    cmd_1.undo();
    myAccount.printStats();

    cmd_3.call();
    myAccount.printStats();

    return 0;
}
