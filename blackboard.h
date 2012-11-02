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

    void runMarket(bool retainData=false);
    void penalize();
    const std::vector<TradeGame::Trade>& getLastBids() const {return lastBids;}
    const std::vector<TradeGame::Trade>& getHistory() const {return history;}
    const std::vector<TradeGame::Trade>& getLastHistory() const {return lastHistory;}
    const std::map<unsigned int,TradeGame::Agent*>& getAgents() const {return agents;}
    void setR(double r) {rFactor = r;}
    void restart();

    static int calculateAssetValue(const TradeGame::Assets& assets);

    static const int SILVER_VALUE = 1;
    static const int GOLD_VALUE = 3;
    static const int PLATINUM_VALUE = 6;

private:
    void init();
    void addAgent(TradeGame::Agent* agent);
    TradeGame::Assets generateAssets() const;
    void inviteBidsAndAddToFloor();
    bool validateBid(const TradeGame::Bid& bid, const TradeGame::Assets& assets);
    void presentBids(const bool retainData=false);
    TradeGame::Trade resolveConflict(const TradeGame::Trade& first, const TradeGame::Trade& second) const;
    void finalizeTrade(const TradeGame::Trade& trade, const bool retainData=false);
    bool verifyAssets(TradeGame::AssetType type, int amount, TradeGame::Agent* agent) const;
    TradeGame::Assets createSellerChange(const TradeGame::Bid& bid);

    static const int TOTAL_ASSET_VALUE = 100;

    double rFactor;
    int iterations;
    unsigned int insertIndex;
    std::map<unsigned int,TradeGame::Agent*> agents;
    //std::map<unsigned int,TradeGame::Assets> initAssets;
    TradingFloor* floor;
    BidList* bidList;
    std::vector<TradeGame::Trade> lastBids;
    std::vector<TradeGame::Trade> history;
    std::vector<TradeGame::Trade> lastHistory;
};

#endif // BLACKBOARD_H
