#include "blackboard.h"
#include "testagent.h"

BlackBoard::BlackBoard(QObject * parent) :
    QObject(parent)
{
    init();
    runMarket();
}

BlackBoard::~BlackBoard()
{
    delete floor;
    delete bidList;
    for(std::map<unsigned int,TradeGame::Agent*>::iterator it = agents.begin(); it!=agents.end(); it++)
    {
        delete it->second;
    }
}

void BlackBoard::init()
{
    floor = new TradingFloor();
    bidList = new BidList();
    for(unsigned int i=0; i<100; i++)
    {
        agents[i] = new TestAgent();
    }
}

void BlackBoard::runMarket()
{
    //init agents, create bidlist
    bidList->create(agents);

    //invite bids
    inviteBidsAndAddToFloor();

    //present bids
    presentBids();

    //penalize
    penalize();
}

void BlackBoard::inviteBidsAndAddToFloor()
{
    floor->clear();
    BidList::Iterator it = bidList->iterate();
    while(it.hasNext())
    {
        unsigned int agent = it.next();
        TradeGame::Bid bid = agents[agent]->inviteBid();
        floor->addBid(bid, agent);
    }
}

void BlackBoard::presentBids()
{
    BidList::Iterator it = bidList->iterate();
    while(it.hasNext())
    {
        unsigned int bidder = it.next();
        std::vector<TradeGame::Trade> matches = floor->checkMatchingBids(bidder);
        if(matches.empty())
        {
            continue;
        }
        std::vector<TradeGame::Bid> simplified;
        simplified.reserve(matches.size());
        for(std::vector<TradeGame::Trade>::const_iterator jt=matches.begin(); jt!=matches.end(); jt++)
        {
            simplified.push_back(jt->bid);
        }
        unsigned int selected = agents[bidder]->selectBestMatch(simplified);
        TradeGame::Trade selectedTrade = matches.at(selected);
        TradeGame::Trade bid = floor->getTrade(bidder);
        TradeGame::Trade finalTrade = resolveConflict(bid, selectedTrade);
        floor->removeBids(finalTrade.seller, finalTrade.buyer);
        it.remove(finalTrade.seller, finalTrade.buyer);

        finalizeTrade(finalTrade);
    }
}

TradeGame::Trade BlackBoard::resolveConflict(const TradeGame::Trade& seller, const TradeGame::Trade& buyer) const
{
    TradeGame::Trade final = seller;
    final.buyer = buyer.seller;
    final.bid.buyingVolume = (seller.bid.buyingVolume + buyer.bid.sellingVolume) / 2;
    final.bid.sellingVolume = (seller.bid.sellingVolume + buyer.bid.sellingVolume) / 2;
    return final;
}

void BlackBoard::finalizeTrade(TradeGame::Trade& trade)
{
    TradeGame::Agent* seller = agents[trade.seller];
    TradeGame::Agent* buyer = agents[trade.buyer];

    //verify assets
    if(!verifyAssets(trade.bid.buyingType, trade.bid.buyingVolume, buyer) ||
       !verifyAssets(trade.bid.sellingType, trade.bid.sellingVolume, seller))
        return;

    //transfer
    TradeGame::Assets sellerChange = createSellerChange(trade.bid);
    seller->addAssets(sellerChange.silver, sellerChange.gold, sellerChange.platinum);
    buyer->addAssets(-sellerChange.silver, -sellerChange.gold, -sellerChange.platinum);

    //history
    history.push_back(trade);
}

bool BlackBoard::verifyAssets(TradeGame::AssetType type, int amount, TradeGame::Agent *agent) const
{
    switch(type)
    {
    case TradeGame::SILVER:
        return agent->getAssets().silver >= amount;
    case TradeGame::GOLD:
        return agent->getAssets().gold >= amount;
    case TradeGame::PLATINUM:
        return agent->getAssets().platinum >= amount;
    }
    return false;
}

TradeGame::Assets BlackBoard::createSellerChange(TradeGame::Bid &bid)
{
    TradeGame::Assets assets = {0,0,0};
    switch(bid.sellingType)
    {
    case TradeGame::SILVER:
        assets.silver -= bid.sellingVolume;
        break;
    case TradeGame::GOLD:
        assets.gold -= bid.sellingVolume;
        break;
    case TradeGame::PLATINUM:
        assets.platinum -= bid.sellingVolume;
        break;
    }
    switch(bid.buyingType)
    {
    case TradeGame::SILVER:
        assets.silver += bid.buyingVolume;
        break;
    case TradeGame::GOLD:
        assets.gold += bid.buyingVolume;
        break;
    case TradeGame::PLATINUM:
        assets.platinum += bid.buyingVolume;
        break;
    }
    return assets;
}

void BlackBoard::penalize()
{
    BidList::Iterator it = bidList->iterate();
    while(it.hasNext())
    {
        TradeGame::Agent* agent = agents[it.next()];
        TradeGame::Assets assets = agent->getAssets();
        TradeGame::Assets penalty = {0,0,0};
        //TODO
        agent->addAssets(penalty.silver, penalty.gold, penalty.platinum);
    }
}
