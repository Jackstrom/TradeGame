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
    const std::vector<TradeGame::Trade>& getHistory() const {return history;}

private:
    void init();
    void inviteBidsAndAddToFloor();
    void presentBids();
    TradeGame::Trade resolveConflict(const TradeGame::Trade& first, const TradeGame::Trade& second) const;
    void finalizeTrade(TradeGame::Trade& trade);
    bool verifyAssets(TradeGame::AssetType type, int amount, TradeGame::Agent* agent) const;
    TradeGame::Assets createSellerChange(TradeGame::Bid& bid);
    void penalize();


    std::map<unsigned int,TradeGame::Agent*> agents;
    TradingFloor* floor;
    BidList* bidList;
    std::vector<TradeGame::Trade> history;
};

#endif // BLACKBOARD_H
