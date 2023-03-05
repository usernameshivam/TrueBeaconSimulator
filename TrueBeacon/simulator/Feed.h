#pragma once
#include <queue>
#include "Tick.h"
#include <mutex>
#include "Order.h"

void processFeed();

inline std::mutex tickQueue_mutex;
inline std::queue<std::shared_ptr<Tick>> tickQueue;

inline std::unordered_map<std::string, std::shared_ptr<Tick>> tickerFeedMap;
