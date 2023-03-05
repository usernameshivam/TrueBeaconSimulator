#include <fstream>
#include "Tick.h"

void TickStore::readTicksFromFile(std::string filename)
{
    //GlobalTicker	Date	LTP	BuyPrice	BuyQty	SellPrice	SellQty	LTQ	OpenInterest	Underlying	ExpiryDate	OptionType	StrikePrice	Ticker

    constexpr char commaDelimiter = ',', spaceDelimiter = ' ';
    size_t delimiterPosition = std::string::npos;
    std::string readline{};
    std::ifstream infobj;

    infobj.open(filename);
    if (!infobj)
    {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }
    std::getline(infobj, readline);
    while (std::getline(infobj, readline))
    {
        std::shared_ptr<Tick> tickDetail = std::make_shared<Tick>();
        delimiterPosition = std::string::npos;
        
        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->globalTicker_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->date_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->ltp_ = std::stod(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);
            
        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->buyPrice_ = std::stod(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->buyQty_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->sellPrice_ = std::stod(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->sellQty_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);

        tickDetail->ltq_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->openInterest_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->underlying_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->expiryDate_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->optionType_ = readline.substr(0, delimiterPosition);
        readline.erase(0, delimiterPosition + 1);

        delimiterPosition = readline.find(commaDelimiter);
        tickDetail->strikePrice_ = std::stoul(readline.substr(0, delimiterPosition));
        readline.erase(0, delimiterPosition + 1);

        tickDetail->ticker_ = readline;
        
        uint32_t time;
        delimiterPosition = tickDetail->date_.find(spaceDelimiter);
        //time = std::stoi(tickDetail->date_.substr(delimiterPosition + 1, 2));
        //time = time * 100 + std::stoi(tickDetail->date_.substr(delimiterPosition + 4, 2));
        //time = time * 100 + std::stoi(tickDetail->date_.substr(delimiterPosition + 7, 2));

        time = std::stoi(tickDetail->date_.substr(delimiterPosition + 1, 2)) * 60 * 60;
        time += std::stoi(tickDetail->date_.substr(delimiterPosition + 4, 2)) * 60;
        time += std::stoi(tickDetail->date_.substr(delimiterPosition + 7, 2));

        timeOrderedTicks_[time].push_back(tickDetail);
        
    }
    infobj.close();

    /*for (const auto &orderedTicksItem : timeOrderedTicks_)
    {
        std::cout << orderedTicksItem.first << std::endl;
    }*/
}

std::map<uint32_t, std::vector<std::shared_ptr<Tick>>> TickStore::getTimeOrderedTicks() const
{
    return timeOrderedTicks_;
}