#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

enum class OrderType : u_short
{
    MARKET = 0,
    LIMIT
};

enum class RequestType : u_short
{
    PLACE_ORDER = 0,
    ORDER_HISTORY_REQUEST,
    ORDER_HISTORY_RESPONSE
};

enum class OrderState : u_short
{
    TRADED = 0,
    CANCELLED
};

enum class BuySellIndicator : u_short
{
    BUY = 0,
    SELL
};

struct Header
{
    RequestType reqType_;
    size_t msgSize_;    
};

struct Order
{
    Header header_;
    char instrument_[30];
    int quantity_;
    OrderType type_;
    int limit_price_;
    BuySellIndicator buy_sell_indicator_;

    Order() : quantity_(0), type_(OrderType::MARKET), limit_price_(0), buy_sell_indicator_(BuySellIndicator::BUY)
    {
        header_.reqType_ = RequestType::PLACE_ORDER;
        header_.msgSize_ = sizeof(Order);
        memset(instrument_, '\0', 30);
    }

    std::string getString() const {
        return std::to_string((u_short)header_.reqType_) + " " + std::to_string(header_.msgSize_) + " " + std::string(instrument_) + " "
        + std::to_string(quantity_) + " " + std::to_string((u_short)type_) + " " + std::to_string(limit_price_)
        + " " + std::to_string((u_short)buy_sell_indicator_);
    }
};

struct DownloadOrderTradeHistory
{
    Header header_;
};

struct Trade
{
    Header header_;
    char instrument_[30];
    int quantity_;
    OrderType type_;
    OrderState state_;
    int price_;
    int fill_price_;
    uint32_t timeStamp_;
    BuySellIndicator buy_sell_indicator_;

    Trade() : quantity_(0), type_(OrderType::MARKET), state_(OrderState::CANCELLED), price_(0), fill_price_(0), timeStamp_(0), buy_sell_indicator_(BuySellIndicator::BUY)
    {
        header_.reqType_ = RequestType::ORDER_HISTORY_RESPONSE;
        header_.msgSize_ = sizeof(Trade);
        memset(instrument_, '\0', 30);
    }

    std::string getString() const {
        return std::to_string((u_short)header_.reqType_) + " " + std::to_string(header_.msgSize_) + " " + std::string(instrument_) + " "
        + std::to_string(quantity_) + " " + std::to_string((u_short)type_) + " " + std::to_string((u_short)state_) + " " + std::to_string(price_)
        + " " + std::to_string(fill_price_) + " " + std::to_string(timeStamp_) + " " + std::to_string((u_short)buy_sell_indicator_);
    }
};

struct TradeHistoryDetails
{
    std::vector<Trade> trades_;
};

void startReadingRequest();