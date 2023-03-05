#include <iostream>
#include <boost/asio.hpp>

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
std::string make_string(boost::asio::streambuf& streambuf)
{
  return {boost::asio::buffers_begin(streambuf.data()), 
          boost::asio::buffers_end(streambuf.data())};
}


int main() {
     boost::asio::io_service io_service;
//socket creation
    ip::tcp::socket socket(io_service);
//connection
     socket.connect( ip::tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));
// request/message from client
     
     boost::system::error_code error;
     
 // getting response from server
    std::string pendingTick = "";
    while(true)
    {
        boost::asio::streambuf read_buffer;
        read_buffer.prepare(sizeof(Tick));
        auto bytes_transferred = boost::asio::read_until(socket,
                                                         read_buffer, '\n');
        read_buffer.commit(bytes_transferred);
        // std::cout << "Read: " << make_string(read_buffer) << " " << bytes_transferred << " " << sizeof(Tick) << std::endl;

        std::string str = make_string(read_buffer).c_str();
        std::vector<std::string> tickString;
        std::string s;
        std::istringstream f(str);
        while (getline(f, s, '\n'))
        {
            // std::cout << "T1: " << s << std::endl;
            tickString.push_back(s);
        }
        size_t tickStringSize = tickString.size();
        for (size_t idx{}; idx < tickStringSize; ++idx)
        {
            if (idx == 0 && pendingTick != "")
            {
                std::clog << (pendingTick + tickString[idx]) << std::endl;
                pendingTick = "";
            }
            else if (idx == tickStringSize - 1)
            {
                auto cnt = std::count(tickString[idx].begin(), tickString[idx].end(), ',');
                if (cnt < 13 || (cnt == 13 && str[str.size() - 1] != '\n'))
                {
                    pendingTick = tickString[idx];
                }
                else
                {
                    std::clog << tickString[idx] << std::endl;
                }
            }
            else
            {
                std::clog << tickString[idx] << std::endl;
            }
        }
        
        read_buffer.consume(bytes_transferred);
    }
    return 0;
}
