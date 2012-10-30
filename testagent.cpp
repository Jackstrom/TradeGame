#include "testagent.h"

TestAgent::TestAgent()
{
}

bool TestAgent::start()
{
    return true;
}

TradeGame::Bid TestAgent::inviteBid()
{
    TradeGame::Bid bid;
    return bid;
}

unsigned int TestAgent::selectBestMach(const std::vector<TradeGame::Bid>& bids)
{
    return 0;
}

void TestAgent::addAssets(int dSilver, int dGold, int dPlatinum)
{

}

TradeGame::Assets TestAgent::getAssets() const
{
    TradeGame::Assets assets;
    return assets;
}
