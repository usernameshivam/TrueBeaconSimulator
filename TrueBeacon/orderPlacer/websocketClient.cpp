#include <iostream>
#include "Order.h"
#include <boost/asio.hpp>
#include <thread>
using namespace boost::asio;

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
        + "," + ticker_ + "\n";
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
std::string make_string(boost::asio::streambuf &streambuf)
{
    return {boost::asio::buffers_begin(streambuf.data()),
            boost::asio::buffers_end(streambuf.data())};
}

void readData(ip::tcp::socket &socket)
{
    while(true)
    {
        boost::asio::streambuf read_buffer;
        auto bytes_transferred = boost::asio::read(socket, read_buffer, 
            boost::asio::transfer_exactly(sizeof(Header)));
        read_buffer.commit(bytes_transferred);
        //std::clog << " received bytes: " << bytes_transferred << std::endl;
        const Header* headerData = boost::asio::buffer_cast<const Header*>(read_buffer.data());
        read_buffer.consume(bytes_transferred);

        if (headerData->reqType_ == RequestType::ORDER_HISTORY_RESPONSE)
        {
            auto bytes_transferred = boost::asio::read(socket, read_buffer, 
            boost::asio::transfer_exactly(sizeof(Trade) - sizeof(Header)));
            read_buffer.commit(bytes_transferred);
            //std::clog << " received bytes: " << bytes_transferred << std::endl;
            const Trade* tradeData = boost::asio::buffer_cast<const Trade*>(read_buffer.data());
            read_buffer.consume(bytes_transferred);
            Trade trade;
            memcpy(&trade, tradeData, sizeof(Trade));
            memcpy(&trade.header_, headerData, sizeof(Header));
            std::clog << trade.getString() << std::endl;
        }
    }
}

void send(ip::tcp::socket &socket, void *data, size_t dataSize)
{
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(data, dataSize), boost::asio::transfer_at_least(dataSize));
    if (!error)
    {
        std::cout << "Message Sent successfully!" << std::endl;
    }
    else
    {
        std::cout << "send failed: " << error.message() << std::endl;
    }
}

int main()
{
     boost::asio::io_service io_service;
     // socket creation
     ip::tcp::socket socket(io_service);
     // connection
     socket.connect(ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 6153));
     // request/message from client

     boost::system::error_code error;
     Order marketOrder;
     strcpy(marketOrder.instrument_, "AARTIIND22MARFUT");
     marketOrder.quantity_ = 10;
     marketOrder.limit_price_ = 0;

     Order marketOrder1;
     strcpy(marketOrder1.instrument_, "ADANIPORTS22MARFUT");
     marketOrder1.quantity_ = 10;
     marketOrder1.limit_price_ = 0;

     Order limitOrder;
     strcpy(limitOrder.instrument_, "ASIANPAINT22MAYFUT");
     limitOrder.type_ = OrderType::LIMIT;
     limitOrder.quantity_ = 15;
     limitOrder.limit_price_ = 895;
     limitOrder.buy_sell_indicator_ = BuySellIndicator::SELL;

     DownloadOrderTradeHistory downloadOrderTradeHistory;
     downloadOrderTradeHistory.header_.reqType_ = RequestType::ORDER_HISTORY_REQUEST;
     limitOrder.header_.msgSize_ = sizeof(DownloadOrderTradeHistory);

     send(socket, &marketOrder, sizeof(Order));
     send(socket, &marketOrder1, sizeof(Order));
     send(socket, &limitOrder, sizeof(Order));
     sleep(10);
     send(socket, &downloadOrderTradeHistory, sizeof(DownloadOrderTradeHistory));

     std::thread th(readData, std::ref(socket));
     th.join();

     return 0;
}
