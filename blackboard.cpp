#include "blackboard.h"
#include "testagent.h"
#include <cstdlib>

#include <cstdio>

BlackBoard::BlackBoard(QObject * parent) :
    QObject(parent),
    rFactor(0.2),
    iterations(0),
    insertIndex(0)
{
    init();
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
        addAgent(new TestAgent());
    }
}

void BlackBoard::addAgent(TradeGame::Agent *agent)
{
    TradeGame::Assets assets = generateAssets();
    agent->setAssets(assets);
    printf("Agent %u initialized with assets %i, %i, %i\n", insertIndex, assets.silver, assets.gold, assets.platinum);
    initAssets[insertIndex] = assets;
    agents[insertIndex++] = agent;
}

TradeGame::Assets BlackBoard::generateAssets() const
{
    TradeGame::Assets assets = {0,0,0};
    int remainingValue = TOTAL_ASSET_VALUE;
    int startAsset = rand() % 2;
    if(startAsset) // start with platinum
    {
        assets.platinum = rand() % (remainingValue / PLATINUM_VALUE + 1);
        remainingValue -= assets.platinum * PLATINUM_VALUE;
        assets.gold = rand() % (remainingValue / GOLD_VALUE + 1);
        remainingValue -= assets.gold * GOLD_VALUE;
    }
    else // start with gold
    {
        assets.gold = rand() % (remainingValue / GOLD_VALUE + 1);
        remainingValue -= assets.gold * GOLD_VALUE;
        assets.platinum = rand() % (remainingValue / PLATINUM_VALUE + 1);
        remainingValue -= assets.platinum * PLATINUM_VALUE;
    }
    assets.silver = remainingValue / SILVER_VALUE; // dump the rest into silver

    return assets;
}

void BlackBoard::runMarket()
{
    iterations++;

    //init agents, create bidlist
    bidList->create(agents);

    //invite bids
    inviteBidsAndAddToFloor();

    //present bids
    presentBids();
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
        int selected = agents[bidder]->selectBestMatch(simplified);
        if(selected < 0 || selected > int(simplified.size()))
            continue;
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
    if(history.empty())
        return;
    printf("\nPenalties:\n");
    for(std::map<unsigned int,TradeGame::Agent*>::const_iterator it=agents.begin(); it!=agents.end(); it++)
    {
        int nrDeals = 0;
        for(std::vector<TradeGame::Trade>::const_iterator itH=history.begin(); itH!=history.end(); itH++)
        {
            if(itH->seller == it->first || itH->buyer == it->first)
                nrDeals++;
        }
        //int totalDeals = history.size();
        TradeGame::Agent* agent = it->second;
        int silverLoss = -rFactor * (1.0 - (float(nrDeals) / iterations)) * initAssets[it->first].silver;
        int goldLoss = -rFactor * (1.0 - float(nrDeals) / iterations) * initAssets[it->first].gold;
        int platinumLoss = -rFactor * (1.0 - float(nrDeals) / iterations) * initAssets[it->first].platinum;

        printf("Penalty for agent %s (%i / %i deals): %i, %i, %i\n", it->second->getName().c_str(), nrDeals, iterations, silverLoss, goldLoss, platinumLoss);
        agent->addAssets(silverLoss, goldLoss, platinumLoss);
    }
    printf("\nFinal assets:\n");
    for(std::map<unsigned int,TradeGame::Agent*>::const_iterator it=agents.begin(); it!=agents.end(); it++)
    {
        TradeGame::Assets assets = it->second->getAssets();
        printf("Assets for agent %s: %i, %i, %i\n", it->second->getName().c_str(), assets.silver, assets.gold, assets.platinum);
    }
}
