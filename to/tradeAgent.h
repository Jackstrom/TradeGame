#ifndef TRADEAGENT_H
#define TRADEAGENT_H

#include "agent.h"
#include <stdlib.h>

namespace DNAOT
{

enum tradePolicy
{
    GREEDY, NEUTRAL, STEALTHY
};

//typedef TradeGame::GOLD GOLD;
//typedef TradeGame::SILVER SILVER;
//typedef TradeGame::PLATINUM PLATINUM;

typedef TradeGame::AssetType collectPolicy;
typedef TradeGame::AssetType AssetType;
typedef TradeGame::Assets AssetSale;
typedef TradeGame::Assets AssetPurchase;


typedef TradeGame::Bid Bid;
typedef TradeGame::Assets Assets;



class Agent : public TradeGame::Agent
{
public:
    Agent(std::vector<unsigned int> dna);

    std::vector<unsigned int> getDna()
    {
        return dna_;
    }

    ~Agent();

    bool start();
    Bid inviteBid();

    int selectBestMatch(const std::vector<Bid> &bids);
    void addAssets(int dSilver, int dGold, int dPlatinum);

    Assets getAssets() const;
    void setAssets(const Assets &assets);
    const std::string getName() const;
    unsigned int calculateScore();

private:

    static unsigned int random(unsigned int max)
    {
        return rand()%(max + 1);
    }


    TradeGame::AssetType selectSellingType();


    unsigned int cutVolume(Bid b)
    {
        unsigned int volume;
        if(b.sellingType == TradeGame::SILVER)
        {
            volume = assets_.silver;
        }else if(b.sellingType == TradeGame::GOLD)
        {
            volume = assets_.gold;

        }else if(b.sellingType == TradeGame::PLATINUM)
        {
            volume = assets_.platinum;
        }

        if(b.sellingVolume > volume)
        {
            return volume;
        }

        return b.sellingVolume;
    }


    //gene data
    collectPolicy currentCollectPolicy_;
    tradePolicy currentTradePolicy_;
    AssetSale currentBuyPolicy_;
    AssetSale currentSellPolicy_;


    //other data
    tradePolicy observedTradePolicy_;
    Assets assets_;

    static const int SILVER_VALUE = 1;
    static const int GOLD_VALUE = 3;
    static const int PLATINUM_VALUE = 6;
    std::vector<unsigned int> dna_;


};
} // end of namespace

#endif // TRADEAGENT_H

