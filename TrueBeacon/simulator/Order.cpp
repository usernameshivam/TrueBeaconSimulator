#include "Order.h"
#include "Feed.h"
#include <boost/asio.hpp>

using namespace boost::asio;

void send(ip::tcp::socket &socket, const void *data, const size_t &dataSize)
{
    boost::system::error_code error;
    auto bytes_transferred = boost::asio::write(socket, boost::asio::buffer(data, dataSize), boost::asio::transfer_at_least(dataSize));
    if (!error)
    {
        std::cout << "Message Sent successfully! bytes: " << bytes_transferred << std::endl;
    }
    else
    {
        std::cout << "send failed: " << error.message() << std::endl;
    }
}

void processOrder(Order &ord)
{
    if (tickerFeedMap.find(ord.instrument_) == tickerFeedMap.end())
    {
        if (ord.type_ == OrderType::MARKET)
        {
            Trade t;
            strcpy(t.instrument_, ord.instrument_);
            t.quantity_ = ord.quantity_;
            t.type_ = ord.type_;
            t.state_ = OrderState::CANCELLED;
            t.price_ = 0;
            t.fill_price_ = 0;
            t.timeStamp_ = time(NULL);
            t.buy_sell_indicator_ = ord.buy_sell_indicator_;
            tradeHistDet.trades_.push_back(t);
        }
        return;
    }
    auto &tick = tickerFeedMap[ord.instrument_];
    if (ord.type_ == OrderType::MARKET)
    {
        if (ord.buy_sell_indicator_ == BuySellIndicator::BUY)
        {
            if (tick->sellPrice_ > 0)
            {
                Trade t;
                strcpy(t.instrument_, ord.instrument_);
                if (ord.quantity_ <= tick->sellQty_)
                {
                    t.quantity_ = ord.quantity_;
                    ord.quantity_ = 0;
                }
                else
                {
                    t.quantity_ = tick->sellQty_;
                    ord.quantity_ -= tick->sellQty_;
                }
                t.type_ = ord.type_;
                t.state_ = OrderState::TRADED;
                t.price_ = tick->sellPrice_;
                t.fill_price_ = tick->sellPrice_;
                t.timeStamp_ = time(NULL);
                t.buy_sell_indicator_ = ord.buy_sell_indicator_;
                tradeHistDet.trades_.push_back(t);
                if (ord.quantity_ > 0)
                {
                    Trade pendingTrade{t};
                    pendingTrade.state_ = OrderState::CANCELLED;
                    pendingTrade.price_ = 0;
                    pendingTrade.fill_price_ = 0;
                    pendingTrade.timeStamp_ = time(NULL);
                    pendingTrade.quantity_ = ord.quantity_;
                    tradeHistDet.trades_.push_back(pendingTrade);
                }
            }       
        }
        else
        {
            if (tick->buyPrice_ > 0)
            {
                Trade t;
                strcpy(t.instrument_, ord.instrument_);
                if (ord.quantity_ <= tick->buyQty_)
                {
                    t.quantity_ = ord.quantity_;
                    ord.quantity_ = 0;
                }
                else
                {
                    t.quantity_ = tick->buyQty_;
                    ord.quantity_ -= tick->buyQty_;
                }
                t.type_ = ord.type_;
                t.state_ = OrderState::TRADED;
                t.price_ = tick->buyPrice_;
                t.fill_price_ = tick->buyPrice_;
                t.timeStamp_ = time(NULL);
                t.buy_sell_indicator_ = ord.buy_sell_indicator_;
                tradeHistDet.trades_.push_back(t);
                if (ord.quantity_ > 0)
                {
                    Trade pendingTrade{t};
                    pendingTrade.state_ = OrderState::CANCELLED;
                    pendingTrade.price_ = 0;
                    pendingTrade.fill_price_ = 0;
                    pendingTrade.timeStamp_ = time(NULL);
                    pendingTrade.quantity_ = ord.quantity_;
                    tradeHistDet.trades_.push_back(pendingTrade);
                }
            }
        }
    }
    else if (ord.type_ == OrderType::LIMIT)
    {
        if (ord.buy_sell_indicator_ == BuySellIndicator::BUY)
        {
            if (tick->sellPrice_ > 0 && tick->sellPrice_ <= ord.limit_price_)
            {
                Trade t;
                strcpy(t.instrument_, ord.instrument_);
                if (ord.quantity_ <= tick->sellQty_)
                {
                    t.quantity_ = ord.quantity_;
                    ord.quantity_ = 0;
                }
                else
                {
                    t.quantity_ = tick->sellQty_;
                    ord.quantity_ -= tick->sellQty_;
                }
                t.type_ = ord.type_;
                t.state_ = OrderState::TRADED;
                t.price_ = tick->sellPrice_;
                if (tick->sellPrice_ < ord.limit_price_)
                {
                    t.fill_price_ = tick->sellPrice_;
                }
                else
                {
                    t.fill_price_ = tick->sellPrice_;
                }
                t.timeStamp_ = time(NULL);
                t.buy_sell_indicator_ = ord.buy_sell_indicator_;
                tradeHistDet.trades_.push_back(t);
            }       
        }
        else
        {
            if (tick->buyPrice_ > 0 && tick->buyPrice_ >= ord.limit_price_)
            {
                Trade t;
                strcpy(t.instrument_, ord.instrument_);
                if (ord.quantity_ <= tick->buyQty_)
                {
                    t.quantity_ = ord.quantity_;
                    ord.quantity_ = 0;
                }
                else
                {
                    t.quantity_ = tick->buyQty_;
                    ord.quantity_ -= tick->buyQty_;
                }
                t.type_ = ord.type_;
                t.state_ = OrderState::TRADED;
                t.price_ = tick->buyPrice_;
                if (tick->buyPrice_ > ord.limit_price_)
                {
                    t.fill_price_ = tick->buyPrice_;
                }
                else
                {
                    t.fill_price_ = tick->buyPrice_;
                }
                t.timeStamp_ = time(NULL);
                t.buy_sell_indicator_ = ord.buy_sell_indicator_;
                tradeHistDet.trades_.push_back(t);
            }
        }
    }
}

void processDownloadOrderRequest(ip::tcp::socket &socket)
{
    for (const auto &trade : tradeHistDet.trades_)
    {
        send(socket, &trade, sizeof(Trade));
    }
    /*Trade trade1;
    strcpy(trade1.instrument_, "ADANIPORTS22MAR550PE");
    trade1.type_ = OrderType::LIMIT;
    trade1.quantity_ = 2;
    trade1.price_ = 589;
    trade1.fill_price_=689;
    trade1.state_ = OrderState::TRADED;
    trade1.buy_sell_indicator_ = BuySellIndicator::SELL;
    trade1.timeStamp_ = time(NULL);
    std::clog << "Sending trade: " << trade1.getString() << std::endl;
    send(socket, &trade1, sizeof(Trade));
    Trade trade2;
    send(socket, &trade2, sizeof(Trade));*/
}

void startReadingRequest()
{
    boost::asio::io_service io_service;
//listen for new connection
    ip::tcp::acceptor acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), 6153 ));
//socket creation
    ip::tcp::socket socket(io_service);
//waiting for connection
    acceptor_.accept(socket);


    while(true)
    {
        boost::asio::streambuf read_buffer;
        auto bytes_transferred = boost::asio::read(socket, read_buffer, 
            boost::asio::transfer_exactly(sizeof(Header)));
        read_buffer.commit(bytes_transferred);
        // std::cout << "Read: " << make_string(read_buffer) << " " << bytes_transferred << " " << sizeof(Tick) << std::endl;

        const Header* headerData = boost::asio::buffer_cast<const Header*>(read_buffer.data());
        read_buffer.consume(bytes_transferred);

        if (headerData->reqType_ == RequestType::PLACE_ORDER)
        {
            auto bytes_transferred = boost::asio::read(socket, read_buffer, 
            boost::asio::transfer_exactly(sizeof(Order) - sizeof(Header)));
            read_buffer.commit(bytes_transferred);
            
            const Order* orderData = boost::asio::buffer_cast<const Order*>(read_buffer.data());
            read_buffer.consume(bytes_transferred);
            Order ord;
            memcpy(&ord, orderData, sizeof(Order));
            memcpy(&ord.header_, headerData, sizeof(Header));
            processOrder(ord);
            if (ord.type_ == OrderType::LIMIT && ord.quantity_ > 0)
            {
                pendingOrders[ord.instrument_].push_back(ord);
            }
            std::clog << ord.getString() << std::endl;
        }
        else if (headerData->reqType_ == RequestType::ORDER_HISTORY_REQUEST)
        {
            processDownloadOrderRequest(socket);
            std::clog << "Download Order request Received" << std::endl;
        }

    }
}