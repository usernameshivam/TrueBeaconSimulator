#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <memory>

struct Tick
{
    std::string globalTicker_;
    std::string date_;
    double ltp_;
    double buyPrice_;
    ulong buyQty_;
    double sellPrice_;
    ulong sellQty_;
    ulong ltq_;
    ulong openInterest_;
    std::string underlying_;
    std::string expiryDate_;
    std::string optionType_;
    double strikePrice_;
    std::string ticker_;

    std::string getString() const
    {
        return globalTicker_ + "," + date_ + "," + std::to_string(ltp_) + "," + std::to_string(buyPrice_)
        + "," + std::to_string(buyQty_) + "," + std::to_string(sellPrice_) + "," + std::to_string(sellQty_)
        + "," + std::to_string(ltq_) + "," + std::to_string(openInterest_) + "," + underlying_
        + "," + expiryDate_ + "," + optionType_ + "," + std::to_string(strikePrice_) 
        + "," + ticker_ + '\n';
    }

    void constructFromString(std::string readline)
    {
        constexpr char commaDelimiter = ',', spaceDelimiter = ' ';
        size_t delimiterPosition = std::string::npos;
        delimiterPosition = std::string::npos;

        delimiterPosition = readline.find(commaDelimiter);
        globalTicker_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        date_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        ltp_ = std::stod(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);
            
        delimiterPosition = readline.find(commaDelimiter);
        buyPrice_ = std::stod(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        buyQty_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        sellPrice_ = std::stod(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        sellQty_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        ltq_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        openInterest_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        underlying_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        expiryDate_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        optionType_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        strikePrice_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find('\n');
        ticker_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);
    }
};

class TickStore
{
    std::map<uint32_t, std::vector<std::shared_ptr<Tick>>> timeOrderedTicks_;
    public:
        TickStore() = default;
        TickStore(const TickStore &obj) = default;
        void readTicksFromFile(std::string filename);
        std::map<uint32_t, std::vector<std::shared_ptr<Tick>>> getTimeOrderedTicks() const;
};