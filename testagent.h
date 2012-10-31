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

    TradeGame::Assets getAssets() const;

    ~TestAgent(){}
};

#endif // TESTAGENT_H
