#include <iostream>
#include <string>
#include <vector>

class Observable;
struct Observer
{
    virtual ~Observer() = default;
    virtual void update(Observable&) = 0;
};


class Observable
{
public:
    virtual ~Observable() = default;
    virtual void addObserver(Observer* obs)
    {
        observers.push_back(obs);
    }
    virtual void removeObserver(Observer* obs)
    {
        //observers.erase(std::remove(observers.begin(), observers.end(), obs)); // before C++20
        std::erase(observers, obs);
    }
    virtual void notify(Observer* obs)
    {
        obs->update(*this);
    }
    virtual void notifyAll()
    {
        for (auto o : observers)
            o->update(*this);
    }

private:
    std::vector<Observer*> observers;
};


class WeatherProvider : public Observable
{
public:
    void setWeather(int temperatureCelsius, std::string sky)
    {
        tempInCelsius = temperatureCelsius;
        skyStatus = sky;

        notifyAll();
    }

    int getTemperatureInCelsius() { return tempInCelsius; }
    std::string getSkyStatus() { return skyStatus; }

private:
    int tempInCelsius;
    std::string skyStatus;
};


class InformationScreen : public Observer
{
public:
    explicit InformationScreen(WeatherProvider& weather_)
        : weather(weather_)
    {
        weather.addObserver(this);
    }

    ~InformationScreen()
    {
        weather.removeObserver(this);
    }

    void update(Observable& updatedObservable) override
    {
        if (&updatedObservable == &weather)
        {
            printWeatherNow();
        }
    }

    void printWeatherNow()
    {
        std::cout << "Temperature: " << weather.getTemperatureInCelsius() << " degree, " << weather.getSkyStatus() << std::endl;
    }

private:
    WeatherProvider& weather;
};


class SmartHomeControl : public Observer
{
public:
    explicit SmartHomeControl(WeatherProvider& weather_)
        : weather(weather_)
    {
        weather.addObserver(this);
    }

    ~SmartHomeControl()
    {
        weather.removeObserver(this);
    }

    void update(Observable& updatedObservable) override
    {
        if (&updatedObservable == &weather)
        {
            auto temp = weather.getTemperatureInCelsius();
            auto skyStatus = weather.getSkyStatus();

            bool closeTheWindows = widowsOpen && (0 == skyStatus.compare("raining"));
            if (closeTheWindows) closeWindows();

            if (temp >= 25)
            {
                if (!closeTheWindows) openWindows();
                if (heatingSystemON) heatOFF();
            }
            else if (temp < 21)
            {
                if (widowsOpen) closeWindows();
                if (temp < 16)
                {
                    if (!heatingSystemON) heatON();
                }
            }
        }
    }

    void openWindows() { std::cout << "Opening all windows..." << std::endl; widowsOpen = true; }
    void closeWindows() { std::cout << "Closing all windows..." << std::endl;  widowsOpen = false; }
    void heatON() { std::cout << "Warming-up the house..." << std::endl; heatingSystemON = true; }
    void heatOFF() { std::cout << "Stop heating system..." << std::endl; heatingSystemON = false; }

private:
    WeatherProvider& weather;
    bool heatingSystemON {false};
    bool widowsOpen{ false };
};

int main()
{
    WeatherProvider weather;
    InformationScreen screen(weather);
    SmartHomeControl homeControl(weather);

    weather.setWeather(25, "sunny");
    weather.setWeather(15, "cloudy");
    weather.setWeather(10, "raining");
    weather.setWeather(26, "sunny");
    weather.setWeather(26, "raining");
}
