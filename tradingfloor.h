#ifndef TRADINGFLOOR_H
#define TRADINGFLOOR_H

#include <vector>
#include "types.h"

class TradingFloor
{
public:
    TradingFloor();

    void clear();
    void addBid(TradeGame::Bid bid, unsigned int bidder);
    TradeGame::Trade getTrade(unsigned int bidder);
    std::vector<TradeGame::Trade> checkMatchingBids(unsigned int seller);
    void removeBids(unsigned int seller, unsigned int buyer);

private:
    std::vector<TradeGame::Trade> trades;
};

#endif // TRADINGFLOOR_H
