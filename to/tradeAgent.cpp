#include "tradeAgent.h"
#include <QtDebug>

namespace DNAOT
{

Agent::Agent(std::vector<unsigned int> dna) :
    dna_(dna)
{

}

Agent::~Agent(){}

bool Agent::start()
{

    currentCollectPolicy_ = collectPolicy(dna_.at(0)); //0-2
    currentTradePolicy_ =tradePolicy(dna_.at(1)); //0-2

    currentSellPolicy_.silver = dna_.at(2); //0-100
    currentSellPolicy_.gold = dna_.at(3); //0-100
    currentSellPolicy_.platinum = dna_.at(4); //0-100

    currentBuyPolicy_.silver = dna_.at(5); //0-100
    currentBuyPolicy_.gold = dna_.at(6); //0-100
    currentBuyPolicy_.platinum = dna_.at(7); //0-100



    if(currentTradePolicy_ == GREEDY)
    {
        int r = random(9);
        if(r != 9)
        {
            return true; //90% chance
        }
    }

    if(currentTradePolicy_ == NEUTRAL)
    {
        int r = random(1);
        if(r==0)
        {
            return true; //50% chance
        }
    }

    if(currentTradePolicy_ == STEALTHY)
    {
        int r = random(3);
        if(r == 0)
        {
            return true; //25% chance
        }
    }

    return false;
}


AssetType BuyWhat(Assets a, collectPolicy p, float v)
{
    int SILVER_VALUE = 1;
    int GOLD_VALUE = 3;
    int PLATINUM_VALUE = 6;


    if(p == TradeGame::SILVER)
    {
     //   if
    }
    if(p == TradeGame::GOLD)
    {

    }
    else //PLATINUM
    {

    }
}

Bid Agent::inviteBid()
{
    Bid b;
    b.buyingType = currentCollectPolicy_;
    float buyVal = 0;
    float sellVal = 0;
    float sellVol = 0;

    b.sellingType = selectSellingType();

    if(b.sellingType == TradeGame::SILVER)
    {
        sellVal = SILVER_VALUE;
        sellVol = currentSellPolicy_.silver;

    }else if(b.sellingType == TradeGame::GOLD)
    {
        sellVal = GOLD_VALUE;
        sellVol = currentSellPolicy_.gold;
    }else //PLATINUM
    {
        //qDebug() << "random";
        sellVal = PLATINUM_VALUE;
        sellVol = currentSellPolicy_.platinum;
    }

    if(currentCollectPolicy_ == TradeGame::SILVER)
    {
        buyVal = SILVER_VALUE;
        b.buyingVolume = currentBuyPolicy_.silver;
    }else if(currentCollectPolicy_ == TradeGame::GOLD)
    {
        buyVal = GOLD_VALUE;
        b.buyingVolume = currentBuyPolicy_.gold;
    }else //PLATINUM
    {
        buyVal = PLATINUM_VALUE;
        b.buyingVolume = currentBuyPolicy_.platinum;
    }


    if(currentTradePolicy_ == GREEDY)
    {
        float i = (buyVal*1.0f/sellVal*1.0f)*b.buyingVolume;
        qDebug() << "i" << i;
        b.sellingVolume = static_cast<unsigned int>(i*0.5);
    }

    if(currentTradePolicy_ == NEUTRAL)
    {
        float i = (buyVal*1.0f/sellVal*1.0f)*b.buyingVolume;
        b.sellingVolume = static_cast<unsigned int>(i);
    }

    if(currentTradePolicy_ == STEALTHY)
    {
        b.sellingVolume = sellVol;
    }

    b.sellingVolume = cutVolume(b);

    return b;
}

int getPrice(AssetType t)
{
    if(t == TradeGame::SILVER)
    {
        return 1;
    }else if(t == TradeGame::GOLD)
    {
        return 3;
    }

    return 6;
}



int Agent::selectBestMatch(const std::vector<Bid> &bids)
{
    //assume first value is best
    Bid choice = bids.front();
    int pos = 0;
    float value = choice.buyingVolume * getPrice(choice.buyingType) * 1.0f /choice.sellingVolume* getPrice(choice.sellingType)*1.0f;


    for(int i = 1; i < bids.size(); ++i)
    {

        Bid tmp = bids.at(i);
        float v = tmp.buyingVolume * getPrice(tmp.buyingType) * 1.0f /tmp.sellingVolume* getPrice(tmp.sellingType)*1.0f;

        if(v > value)
        {
            pos = i;
        }

    }

    return pos;

}

void Agent::addAssets(int dSilver, int dGold, int dPlatinum)
{
    assets_.silver += dSilver;
    assets_.gold += dGold;
    assets_.platinum += dPlatinum;
}

Assets Agent::getAssets() const
{
    return assets_;
}

void Agent::setAssets(const Assets& assets)
{
    assets_.gold = assets.gold;
    assets_.silver = assets.silver;
    assets_.platinum = assets.platinum;
}

const std::string Agent::getName() const
{
    return "x";
}

unsigned int Agent::calculateScore()
{

    return (assets_.silver * SILVER_VALUE) +
           (assets_.gold * GOLD_VALUE) +
           (assets_.platinum * PLATINUM_VALUE);

}

TradeGame::AssetType Agent::selectSellingType()
{
    if(currentCollectPolicy_ == TradeGame::SILVER)
    {
        int r = random(1);
        if (r != 0)
        {
            return TradeGame::GOLD;
        }
        else
        {
            return TradeGame::PLATINUM;
        }

    }
    if(currentCollectPolicy_ == TradeGame::GOLD)
    {
        int r = random(1);
        if (r != 0)
        {
            return TradeGame::SILVER;
        }
        else
        {
            return TradeGame::PLATINUM;
        }
    }
    if(currentCollectPolicy_ == TradeGame::PLATINUM)
    {
        int r = random(1);

        if (r != 0)
        {
            return TradeGame::SILVER;
        }
        else
        {
            return TradeGame::GOLD;
        }
    }
}


} // end of namespace
