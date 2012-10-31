#include "tradingfloor.h"

#include <QDebug>

TradingFloor::TradingFloor()
{
}

void TradingFloor::clear()
{
    trades.clear();
}

void TradingFloor::addBid(TradeGame::Bid bid, unsigned int bidder)
{
    TradeGame::Trade trade;
    trade.bid = bid;
    trade.seller = bidder;
    trades.push_back(trade);
}

TradeGame::Trade TradingFloor::getTrade(unsigned int bidder)
{
    for(std::vector<TradeGame::Trade>::iterator it=trades.begin(); it!=trades.end(); it++)
    {
        if(it->seller == bidder)
        {
            return *it;
        }
    }
    return TradeGame::Trade();
}

std::vector<TradeGame::Trade> TradingFloor::checkMatchingBids(unsigned int seller)
{
    TradeGame::Trade sellerBid;
    for(std::vector<TradeGame::Trade>::iterator it=trades.begin(); it!=trades.end(); it++)
    {
        if(it->seller == seller)
        {
            sellerBid = *it;
            break;
        }
    }

    std::vector<TradeGame::Trade> matches;
    for(std::vector<TradeGame::Trade>::iterator it=trades.begin(); it!=trades.end(); it++)
    {
        if(sellerBid.bid.buyingType == it->bid.sellingType &&
           sellerBid.bid.sellingType == it->bid.buyingType)
        {
            matches.push_back(*it);
            //printf("Detected match of %i to %i\n", sellerBid.bid.sellingType, sellerBid.bid.buyingType);
        }
    }
    return matches;
}

void TradingFloor::removeBids(unsigned int seller, unsigned int buyer)
{
    std::vector<TradeGame::Trade>::iterator it = trades.begin();
    while(it != trades.end())
    {
        if(it->seller == seller || it->seller == buyer)
        {
            it = trades.erase(it);
            continue;
        }
        it++;
    }
}
