#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "Feed.h"

typedef int32_t packetRateType;

using namespace boost::asio;

std::atomic<packetRateType> packetRate;

void sendToClient(ip::tcp::socket &socket, const std::string& message) {
    boost::asio::write(socket, boost::asio::buffer(message));
}

void publishTickOnInterval(const std::map<uint32_t, std::vector<std::shared_ptr<Tick>>> orderedTicks, ip::tcp::socket &socket)
{
       long prev{}, seconds{};
       auto orderedTickBeginItr = orderedTicks.begin();
       auto prevOrderedTickBeginItr = orderedTicks.end();
       auto prevOrderedTickEndItr = orderedTicks.end();
       uint32_t timeTracker = orderedTickBeginItr->first;
       auto printTicks = [](const auto &itr, ip::tcp::socket & socket)
       {
           for (const auto &tick : itr.second)
           {
                const std::lock_guard<std::mutex> lock(tickQueue_mutex);
                tickQueue.push(tick);
                sendToClient(socket, tick->getString());
               //std::clog << tick->getString();
           }
       };

       while (true)
       {
           seconds = time(nullptr);
           long diff = abs(prev - seconds);
           if (diff >= 1)
           {
               auto orderedTickEndItr = --orderedTicks.upper_bound(timeTracker + (packetRate.load() - 1));
               if (prevOrderedTickBeginItr == orderedTickBeginItr && prevOrderedTickEndItr != orderedTickEndItr)
                   ++orderedTickBeginItr;
               std::for_each(orderedTickBeginItr, orderedTickEndItr, [&](auto &orderedTickItr)
                             { printTicks(orderedTickItr, socket); });
               if (prevOrderedTickEndItr != orderedTickEndItr)
                   printTicks(*orderedTickEndItr, socket);
               else
               {
                    auto &tick = *--orderedTickEndItr->second.end();
                    const std::lock_guard<std::mutex> lock(tickQueue_mutex);
                    tickQueue.push(tick);
                    sendToClient(socket, tick->getString());
                   //std::clog << (*--orderedTickEndItr->second.end())->getString();
               }
               timeTracker += packetRate.load();
               prevOrderedTickBeginItr = orderedTickBeginItr;
               orderedTickBeginItr = --orderedTicks.upper_bound(timeTracker);
               prevOrderedTickEndItr = orderedTickEndItr;
               prev = seconds;
           }
       }
}




int main(int argc, char** arg)
{
    if (argc != 2)
    {
        std::clog << "Usage: " << arg[0] << " <tick filename>" << std::endl;
        return 0;
    }
    TickStore tickStore;
    std::clog << "reading ticks from " << arg[1] << std::endl;
    tickStore.readTicksFromFile(arg[1]);
    std::clog << "reading ticks completed" << std::endl;
    std::clog << "setting packet rate to 1x" << std::endl;
    packetRate.store(1);
    boost::asio::io_service io_service;
//listen for new connection
    ip::tcp::acceptor acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), 1234 ));
//socket creation
    ip::tcp::socket socket(io_service);
//waiting for connection
    acceptor_.accept(socket);
    
    std::clog << "starting tick publishing thread" << std::endl;
    std::thread thFeed(processFeed);
    std::thread th(publishTickOnInterval, tickStore.getTimeOrderedTicks(), std::ref(socket));
    std::thread th1(startReadingRequest);
    std::clog << "Enter packet rate as -1 to stop prompt for packet rate" << std::endl;
    packetRateType newPacketRate{1};
    while (true)
    {
        std::clog << "Enter new packet rate" << std::endl;
        std::cin >> newPacketRate;
        if (newPacketRate == 0)
        {
            break;
        }
        packetRate.store(newPacketRate);
    }
    
    th.join();
    thFeed.join();
    th1.join();
    return 0;
}