#include <iostream>
#include <string>
#include <vector>

class BankAccount
{
    friend class BankAccountCommand;
public:
    explicit BankAccount(const std::string& name_)
    : name(name_)
    {}

    BankAccount(const std::string& name_, unsigned initial_amount)
    : name(name_)
    , balance(initial_amount)
    {}

    void printStats() const
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
    virtual ~Command() = default;
};


class BankAccountCommand : public Command
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
            successfulAction = true;
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
        if (undoneAction || !successfulAction)
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

    bool isSuccessful() const
    {
        return successfulAction;
    }

private:
    BankAccount& account;
    const Action action;
    bool successfulAction {false};
    bool undoneAction {false};
    const unsigned amount;
};


struct CompositeBankAccountCommand : std::vector<BankAccountCommand>, Command
{
    CompositeBankAccountCommand(const std::initializer_list<value_type>& items)
    : std::vector<BankAccountCommand>(items)
    {}

    void call() override
    {
        for (auto& cmd : *this)
            cmd.call();
    }

    void undo() override
    {
        for (auto it = rbegin(); it != rend(); ++it)
            it->undo();
    }
};


struct DependentCompositeCommand : CompositeBankAccountCommand
{
    using CompositeBankAccountCommand::CompositeBankAccountCommand;

    // Breakes chain if a command fails
    void call() override
    {
        for (auto& cmd : *this)
        {
            if (successfulChain)
            {
                cmd.call();
                successfulChain = cmd.isSuccessful();
            }
        }
    }

private:
    bool successfulChain {true};
};


struct BankTransferCommand : DependentCompositeCommand
{
    BankTransferCommand(BankAccount& from, BankAccount& to, unsigned amount)
    : DependentCompositeCommand{BankAccountCommand{from, BankAccountCommand::Action::withdraw, amount},
                                  BankAccountCommand{to, BankAccountCommand::Action::deposit, amount}}
    {}
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

    std::cout << "\n\nMake some bank tranfers..." << std::endl;
    BankAccount account_1{"customer 1", 300};
    BankAccount account_2{"customer 2", 500};
    account_1.printStats();
    account_2.printStats();

    std::cout << "\ntransfer_1: 200 euro from account_1 to account_2" << std::endl;
    BankTransferCommand transfer_1(account_1, account_2, 200);
    transfer_1.call();
    account_1.printStats();
    account_2.printStats();

    std::cout << "\ntransfer_2: 150 euro from account_1 to account_2" << std::endl;
    BankTransferCommand transfer_2(account_1, account_2, 150);
    transfer_2.call(); // will fail
    account_1.printStats();
    account_2.printStats();

    std::cout << "\nundo transfer_2: nothing should happen!" << std::endl;
    transfer_2.undo(); // nothing to undo!
    account_1.printStats();
    account_2.printStats();

    std::cout << "\nundo transfer_1: back to initial state" << std::endl;
    transfer_1.undo(); // nothing to undo!
    account_1.printStats();
    account_2.printStats();

    return 0;
}
