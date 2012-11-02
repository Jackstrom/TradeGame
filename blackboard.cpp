#include "blackboard.h"
#include "testagent.h"
#include <cstdlib>

#include <assert.h>
#include <cstdio>

BlackBoard::BlackBoard(QObject * parent) :
    QObject(parent),
    rFactor(0.2),
    iterations(0),
    insertIndex(0)
{
    floor = new TradingFloor();
    bidList = new BidList();
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
    for(unsigned int i=0; i<100; i++)
    {
        addAgent(new TestAgent());
    }
}

void BlackBoard::restart()
{
    for(std::map<unsigned int,TradeGame::Agent*>::iterator it = agents.begin(); it!=agents.end(); it++)
    {
        delete it->second;
    }
    agents.clear();
    insertIndex = 0;
    history.clear();
    lastHistory.clear();
    lastBids.clear();
    init();
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

void BlackBoard::runMarket(bool retainData)
{
    iterations++;

    // prevent negatives
    for(std::map<unsigned int,TradeGame::Agent*>::iterator it=agents.begin(); it!=agents.end(); it++)
    {
        TradeGame::Assets assets = it->second->getAssets();
        assets.silver = std::max(assets.silver, 0);
        assets.gold = std::max(assets.gold, 0);
        assets.platinum = std::max(assets.platinum, 0);
        it->second->setAssets(assets);
    }

    //init agents, create bidlist
    bidList->create(agents);

    //invite bids
    inviteBidsAndAddToFloor();
    if(retainData)
    {
        lastBids = floor->getTrades();
        lastHistory.clear();
    }

    //present bids
    presentBids(retainData);
}

void BlackBoard::inviteBidsAndAddToFloor()
{
    floor->clear();
    BidList::Iterator it = bidList->iterate();
    while(it.hasNext())
    {
        unsigned int agent = it.next();
        TradeGame::Bid bid = agents[agent]->inviteBid();
        TradeGame::Assets assets = agents[agent]->getAssets();
        if(validateBid(bid, assets))
            floor->addBid(bid, agent);
    }
}

bool BlackBoard::validateBid(const TradeGame::Bid &bid, const TradeGame::Assets &assets)
{
    if(bid.sellingVolume == 0 || bid.buyingVolume == 0)
        return false;
    switch(bid.sellingType)
    {
    case TradeGame::SILVER:
        return assets.silver >= int(bid.sellingVolume);
    case TradeGame::GOLD:
        return assets.gold >= int(bid.sellingVolume);
    case TradeGame::PLATINUM:
        return assets.platinum >= int(bid.sellingVolume);
    default:
        return false;
    }
}

void BlackBoard::presentBids(const bool retainData)
{
    BidList::Iterator it = bidList->iterate();
    while(it.hasNext())
    {
        unsigned int partA = it.next();
        std::vector<TradeGame::Trade> matches = floor->checkMatchingBids(partA);
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
        int selected = agents[partA]->selectBestMatch(simplified);
        if( (selected < 0) || (selected > int(simplified.size())) )
            continue;

        TradeGame::Trade partBTrade = matches.at(selected);
        unsigned int partB = partBTrade.seller;
        TradeGame::Trade partATrade = floor->getTrade(partA);
        TradeGame::Trade finalPartATrade = resolveConflict(partATrade, partBTrade);
/*
        TradeGame::Assets partAPre = agents[partA]->getAssets();
        TradeGame::Assets partBPre = agents[partB]->getAssets();
        */
        //verify assets
        if(!verifyAssets(finalPartATrade.bid.buyingType, finalPartATrade.bid.buyingVolume, agents[partB]) ||
           !verifyAssets(finalPartATrade.bid.sellingType, finalPartATrade.bid.sellingVolume, agents[partA]))
            continue;

        floor->removeBids(partA, partB);
        it.remove(partA, partB);

        finalizeTrade(finalPartATrade, retainData);
/*
        TradeGame::Assets partAPost = agents[partA]->getAssets();
        TradeGame::Assets partBPost = agents[partB]->getAssets();

        assert(partAPost.platinum >= 0 && partAPost.gold >= 0 && partAPost.silver >= 0);
        assert(partBPost.platinum >= 0 && partBPost.gold >= 0 && partBPost.silver >= 0);*/
    }
}

TradeGame::Trade BlackBoard::resolveConflict(const TradeGame::Trade& seller, const TradeGame::Trade& buyer) const
{
    TradeGame::Trade final = seller;
    final.buyer = buyer.seller;
    final.bid.buyingVolume = (seller.bid.buyingVolume + buyer.bid.sellingVolume) / 2;
    final.bid.sellingVolume = (seller.bid.sellingVolume + buyer.bid.buyingVolume) / 2;

    // validate and adjust new transaction against assets
    TradeGame::Assets sellerAssets = agents.find(final.seller)->second->getAssets();
    TradeGame::Assets buyerAssets = agents.find(final.buyer)->second->getAssets();
    switch(final.bid.buyingType)
    {
    case TradeGame::SILVER:
        final.bid.buyingVolume = std::min(int(final.bid.buyingVolume), buyerAssets.silver);
        break;
    case TradeGame::GOLD:
        final.bid.buyingVolume = std::min(int(final.bid.buyingVolume), buyerAssets.gold);
        break;
    case TradeGame::PLATINUM:
        final.bid.buyingVolume = std::min(int(final.bid.buyingVolume), buyerAssets.platinum);
        break;
    }
    switch(final.bid.sellingType)
    {
    case TradeGame::SILVER:
        final.bid.sellingVolume = std::min(int(final.bid.sellingVolume), sellerAssets.silver);
        break;
    case TradeGame::GOLD:
        final.bid.sellingVolume = std::min(int(final.bid.sellingVolume), sellerAssets.gold);
        break;
    case TradeGame::PLATINUM:
        final.bid.sellingVolume = std::min(int(final.bid.sellingVolume), sellerAssets.platinum);
        break;
    }

    return final;
}

void BlackBoard::finalizeTrade(const TradeGame::Trade& trade, const bool retainData)
{
    TradeGame::Agent* seller = agents[trade.seller];
    TradeGame::Agent* buyer = agents[trade.buyer];

    //transfer
    TradeGame::Assets sellerChange = createSellerChange(trade.bid);
    seller->addAssets(sellerChange.silver, sellerChange.gold, sellerChange.platinum);
    buyer->addAssets(-sellerChange.silver, -sellerChange.gold, -sellerChange.platinum);

    //history
    if(retainData)
        lastHistory.push_back(trade);
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

TradeGame::Assets BlackBoard::createSellerChange(const TradeGame::Bid &bid)
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
        TradeGame::Agent* agent = it->second;
        int silverLoss = -rFactor * (1.0 - (float(nrDeals) / iterations)) * initAssets[it->first].silver;
        int goldLoss = -rFactor * (1.0 - float(nrDeals) / iterations) * initAssets[it->first].gold;
        int platinumLoss = -rFactor * (1.0 - float(nrDeals) / iterations) * initAssets[it->first].platinum;

        assert(silverLoss <= 0 && goldLoss <= 0 && platinumLoss <= 0);

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

int BlackBoard::calculateAssetValue(const TradeGame::Assets& assets)
{
    int score = assets.platinum * BlackBoard::PLATINUM_VALUE;
    score += assets.gold * BlackBoard::GOLD_VALUE;
    score += assets.silver * BlackBoard::SILVER_VALUE;
    return score;
}
