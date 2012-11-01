#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <QObject>
#include <vector>
#include <map>
#include "agent.h"
#include "types.h"
#include "tradingfloor.h"
#include "bidlist.h"


class BlackBoard : public QObject
{
    Q_OBJECT

public:
    BlackBoard(QObject * parent=0);
    ~BlackBoard();

    void runMarket();
    void penalize();
    const std::vector<TradeGame::Trade>& getHistory() const {return history;}
    const std::map<unsigned int,TradeGame::Agent*> getAgents() const {return agents;}

    static const int SILVER_VALUE = 1;
    static const int GOLD_VALUE = 3;
    static const int PLATINUM_VALUE = 6;

private:
    void init();
    void addAgent(TradeGame::Agent* agent);
    TradeGame::Assets generateAssets() const;
    void inviteBidsAndAddToFloor();
    void presentBids();
    TradeGame::Trade resolveConflict(const TradeGame::Trade& first, const TradeGame::Trade& second) const;
    void finalizeTrade(TradeGame::Trade& trade);
    bool verifyAssets(TradeGame::AssetType type, int amount, TradeGame::Agent* agent) const;
    TradeGame::Assets createSellerChange(TradeGame::Bid& bid);

    static const int TOTAL_ASSET_VALUE = 100;

    const double rFactor;
    int iterations;
    unsigned int insertIndex;
    std::map<unsigned int,TradeGame::Agent*> agents;
    std::map<unsigned int,TradeGame::Assets> initAssets;
    TradingFloor* floor;
    BidList* bidList;
    std::vector<TradeGame::Trade> history;
};

#endif // BLACKBOARD_H
