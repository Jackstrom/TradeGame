#include "testagent.h"

TestAgent::TestAgent()
{
}

bool TestAgent::start()
{
    return true;
}

TradeGame::AssetType cycleVar()
{
    static bool direction = false;
    static int count = 0;
    if(count++ % 40 == 0)
    {
        direction = !direction;
    }
    static TradeGame::AssetType varType = TradeGame::SILVER;
    if(direction)
    {
        if(varType == TradeGame::SILVER)
            varType = TradeGame::GOLD;
        else if(varType == TradeGame::GOLD)
            varType = TradeGame::PLATINUM;
        else if(varType == TradeGame::PLATINUM)
            varType = TradeGame::SILVER;
        return varType;
    }
    else
    {
        if(varType == TradeGame::SILVER)
            varType = TradeGame::PLATINUM;
        else if(varType == TradeGame::GOLD)
            varType = TradeGame::SILVER;
        else if(varType == TradeGame::PLATINUM)
            varType = TradeGame::GOLD;
        return varType;
    }
}

TradeGame::Bid TestAgent::inviteBid()
{
    static int volume = 1;
    TradeGame::Bid bid;
    bid.buyingType = cycleVar();
    bid.buyingVolume = volume++ % 6;
    bid.sellingType = cycleVar();
    bid.sellingVolume = volume++ % 6;
    return bid;
}

unsigned int TestAgent::selectBestMatch(const std::vector<TradeGame::Bid>& bids)
{
    unsigned int maxRatio = 0;
    unsigned int selected = 0;
    for(unsigned int i=0; i<bids.size(); i++)
    {
        const TradeGame::Bid& bid = bids.at(i);
        float ratio = float(bid.sellingVolume) / bid.buyingVolume;
        if(ratio > maxRatio)
        {
            maxRatio = ratio;
            selected = i;
        }
    }
    return selected;
}

void TestAgent::addAssets(int dSilver, int dGold, int dPlatinum)
{

}

TradeGame::Assets TestAgent::getAssets() const
{
    TradeGame::Assets assets;
    return assets;
}
