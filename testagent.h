#ifndef TESTAGENT_H
#define TESTAGENT_H

#include "agent.h"

class TestAgent : public TradeGame::Agent
{
public:
    TestAgent();

    bool start();  //returns true if it wants to trade.

    TradeGame::Bid inviteBid();

    unsigned int selectBestMatch(const std::vector<TradeGame::Bid>& bids);
    void addAssets(int dSilver, int dGold, int dPlatinum);

    TradeGame::Assets getAssets() const {return assets;}
    void setAssets(const TradeGame::Assets& assets){this->assets = assets;}
    const std::string getName() const {return "TestAgent007";}

    ~TestAgent(){}

private:
    TradeGame::Assets assets;
};

#endif // TESTAGENT_H
