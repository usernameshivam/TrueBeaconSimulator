#include "Feed.h"


void processFeed()
{
    while(true)
    {
        tickQueue_mutex.lock();
        if (!tickQueue.empty())
        {
            auto tick = tickQueue.front();
            tickQueue.pop();
            tickQueue_mutex.unlock();

            tickerFeedMap[tick->ticker_] = tick;

            for (size_t i = 0; i < pendingOrders[tick->ticker_].size(); )
            {
                processOrder(pendingOrders[tick->ticker_][i]);
                if (pendingOrders[tick->ticker_][i].quantity_ == 0)
                {
                    pendingOrders[tick->ticker_].erase(pendingOrders[tick->ticker_].begin() + i);
                    
                }
                else
                {
                    ++i;
                }
            }

        }
        else
        {
            tickQueue_mutex.unlock();
        }
    }
}