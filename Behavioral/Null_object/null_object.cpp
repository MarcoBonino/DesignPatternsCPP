#include <iostream>
#include <memory>
#include <string>

struct Logger
{
    virtual ~Logger() = default;
    virtual void logInfo(const std::string& msg) = 0;
    virtual void logError(const std::string& msg) = 0;
};

struct ConsoleLogger : Logger
{
    void logInfo(const std::string& msg) override
    {
        std::cout << "INFO: " << msg << std::endl;
    }

    void logError(const std::string& msg) override
    {
        std::cout << "ERROR: " << msg << std::endl;
    }
};

/* Null Object */
struct NullLogger : Logger
{
    void logInfo(const std::string& msg) override {}
    void logError(const std::string& msg) override {}
};


class Game
{
public:
    Game(const std::shared_ptr<Logger>& logger_)
    : logger(logger_)
    {}

    void play()
    {
        // code to start the game...
        logger->logInfo("Game started!");
    }

    void handleCrash()
    {
        // code to quit the game due to unexpeced error...
        logger->logError("Game crashed... Quit.");
    }

private:
    std::shared_ptr<Logger> logger;
};


int main(int argc, const char* argv[])
{
    std::shared_ptr<Logger> consoleLog = std::make_shared<ConsoleLogger>();
    Game game1(consoleLog);
    game1.play();
    game1.handleCrash();

    std::shared_ptr<Logger> nullLog = std::make_shared<NullLogger>();
    Game game2(nullLog);
    game2.play();
    game2.handleCrash();
    return 0;
}
