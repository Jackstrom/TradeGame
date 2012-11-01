#ifndef TESTAGENT_H
#define TESTAGENT_H

#include "agent.h"

class TestAgent : public TradeGame::Agent
{
public:
    TestAgent();

    bool start();  //returns true if it wants to trade.

    TradeGame::Bid inviteBid();

    int selectBestMatch(const std::vector<TradeGame::Bid>& bids);
    void addAssets(int dSilver, int dGold, int dPlatinum);

    TradeGame::Assets getAssets() const {return assets;}
    void setAssets(const TradeGame::Assets& assets){this->assets = assets;}
    const std::string getName() const {return name;}

    ~TestAgent(){}

private:
    TradeGame::Assets assets;
    std::string name;
};

#endif // TESTAGENT_H
