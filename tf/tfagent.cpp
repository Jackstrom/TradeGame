#include "tfagent.h"

// for debug
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <QtDebug>

namespace TF
{

static unsigned int idc = 0;

Agent::Agent() :
    id(++idc)
{
    init();
}

Agent::Agent(const Genome &genome) :
    genome(genome),
    id(++idc)
{   
    init();
}

Agent::Agent(unsigned int n_default_genome) :
    id(++idc)
{
    Genome genome;
    switch (n_default_genome)
    {

    case 0:
        genome.willingness = 1;
        genome.collecting_policy = TradeGame::GOLD;
        genome.sell_policy = TradeGame::SILVER;
        genome.trading_policy = TF::Stealthy;
        genome.buy_vol_gold = 9;
        genome.buy_vol_platinum = 2;
        genome.buy_vol_silver = 50;
        genome.sell_vol_gold = 11;
        genome.sell_vol_silver = 3;
        genome.sell_vol_platinum = 17;
        break;

    case 1:
        genome.willingness = 2;
        genome.collecting_policy = TradeGame::PLATINUM;
        genome.sell_policy = TradeGame::SILVER;
        genome.trading_policy = TF::Greedy;
        genome.buy_vol_gold = 7;
        genome.buy_vol_platinum = 40;
        genome.buy_vol_silver = 45;
        genome.sell_vol_gold = 12;
        genome.sell_vol_silver = 28;
        genome.sell_vol_platinum = 9;
        break;

    case 2:
        genome.willingness = 3;
        genome.collecting_policy = TradeGame::PLATINUM;
        genome.sell_policy = TradeGame::SILVER;
        genome.trading_policy = TF::Greedy;
        genome.buy_vol_gold = 10;
        genome.buy_vol_platinum = 13;
        genome.buy_vol_silver = 35;
        genome.sell_vol_gold = 18;
        genome.sell_vol_silver = 35;
        genome.sell_vol_platinum = 10;
        break;

    case 3:
        genome.willingness = 1;
        genome.collecting_policy = TradeGame::PLATINUM;
        genome.sell_policy = TradeGame::SILVER;
        genome.trading_policy = TF::Stealthy;
        genome.buy_vol_gold = 16;
        genome.buy_vol_platinum = 21;
        genome.buy_vol_silver = 48;
        genome.sell_vol_gold = 28;
        genome.sell_vol_silver = 24;
        genome.sell_vol_platinum = 23;
        break;

    case 4:
        genome.willingness = 1;
        genome.collecting_policy = TradeGame::PLATINUM;
        genome.sell_policy = TradeGame::SILVER;
        genome.trading_policy = TF::Stealthy;
        genome.buy_vol_gold = 2;
        genome.buy_vol_platinum = 46;
        genome.buy_vol_silver = 7;
        genome.sell_vol_gold = 19;
        genome.sell_vol_silver = 13;
        genome.sell_vol_platinum = 21;
        break;

    case 5:
        genome.willingness = 2;
        genome.collecting_policy = TradeGame::PLATINUM;
        genome.sell_policy = TradeGame::SILVER;
        genome.trading_policy = TF::Neutral;
        genome.buy_vol_gold = 31;
        genome.buy_vol_platinum = 2;
        genome.buy_vol_silver = 7;
        genome.sell_vol_gold = 14;
        genome.sell_vol_silver = 49;
        genome.sell_vol_platinum = 34;
        break;
    }

    this->genome = genome;
    init();

}

void Agent::init()
{
    prev_assets.gold = rand() % 25;
    prev_assets.silver = rand() % 25;
    prev_assets.platinum = rand() % 25;

    assets.gold = rand() % 25;
    assets.silver = rand() % 25;
    assets.platinum = rand() % 25;

    name = generateName();

    ready = true;
}

bool Agent::start()
{
    return ready;
}

std::string Agent::generateName()
{
    std::string vowels = "eyuioa";
    std::string cons = "qwrtpsdfghklzcvbnm";

    std::locale loc;
    std::stringstream ss;

    ss << "(TF) ";

    for (unsigned int i = 0; i < 8; i++)
    {
        if (i == 0)
            ss << std::toupper(cons[rand() % cons.size()], loc);
        if (i % 2)
            ss << cons.at(rand() % cons.size());
        else
            ss << vowels.at(rand() % vowels.size());
    }

    std::string name_local = ss.str();

    return name_local;
}

const std::string Agent::getName() const
{
    //return genome.toString();
    return name;
}

void Agent::setName(const std::string &name)
{
    this->name = name;
}

void Agent::setGenome(const Genome &genome)
{
    this->genome = genome;
}

Genome Agent::getGenome() const
{
    return genome;
}

unsigned int Agent::getId() const
{
    return id;
}

void Agent::addAssets(int dSilver, int dGold, int dPlatinum)
{
    prev_assets = assets;

    assets.silver += dSilver;
    assets.gold += dGold;
    assets.platinum += dPlatinum;
}

void Agent::setAssets(const TradeGame::Assets &assets)
{
    prev_assets = this->assets;

    this->assets = assets;
}

// TODO: complete, no selection done, returns 0
int Agent::selectBestMatch(const std::vector<TradeGame::Bid> &bids)
{
    std::vector<TradeGame::Bid>::const_iterator it;
    unsigned int match = 0;
    unsigned int i = 0;

    float fitness = 0;
    for (it = bids.begin(); it != bids.end(); it++, i++)
    {
        TradeGame::Bid bid = *it;

        TradeGame::Assets potential = assets;

        switch(bid.buyingType)
        {
            case TradeGame::SILVER:
                potential.silver -= bid.buyingVolume;
                break;
            case TradeGame::GOLD:
                potential.gold -= bid.buyingVolume;
                break;
            case TradeGame::PLATINUM:
                potential.platinum -= bid.buyingVolume;
                break;
            default:
                break;
        }

        switch(bid.sellingType)
        {
            case TradeGame::SILVER:
                potential.silver += bid.sellingVolume;
                break;
            case TradeGame::GOLD:
                potential.gold += bid.sellingVolume;
                break;
            case TradeGame::PLATINUM:
                potential.platinum += bid.sellingVolume;
                break;
            default:
                break;
        }

        float tmp = getFitness(potential, assets);
        if (tmp > fitness)
        {
            fitness = tmp;
            match = i;
        }
    }

    return match;
}

TradeGame::Bid Agent::inviteBid()
{
    return createBid();
}

TradeGame::Bid Agent::createBid()
{
    TradeGame::Bid bid;

    bid.buyingType = genome.collecting_policy;
    switch (bid.buyingType)
    {
        case TradeGame::SILVER:
            bid.buyingVolume = genome.buy_vol_silver;
            break;
        case TradeGame::GOLD:
            bid.buyingVolume = genome.buy_vol_silver;
            break;
        case TradeGame::PLATINUM:
            bid.buyingVolume = genome.buy_vol_platinum;
            break;
        default:
            break;
    }

    bid.sellingType = genome.sell_policy;
    switch (bid.sellingType)
    {
        case TradeGame::SILVER:
            bid.sellingVolume = (genome.sell_vol_silver > assets.silver) ?
                        assets.silver : genome.sell_vol_silver;
            break;
        case TradeGame::GOLD:
            bid.sellingVolume = (genome.sell_vol_gold > assets.gold) ?
                        assets.gold : genome.sell_vol_gold;
            break;
        case TradeGame::PLATINUM:
            bid.sellingVolume = (genome.sell_vol_platinum > assets.platinum) ?
                        assets.platinum : genome.sell_vol_platinum;
            break;
        default:
            break;
    }

    return bid;
}

TradeGame::Assets Agent::getAssets() const
{
    return assets;
}

float Agent::getFitness(TradeGame::Assets &curr_assets,
                        TradeGame::Assets &previous_assets) const
{
    float total_prev_assets = previous_assets.silver +
            previous_assets.gold * 3 +
            previous_assets.platinum * 6;

    float total_curr_assets = curr_assets.silver +
            curr_assets.gold * 3 +
            curr_assets.platinum * 6;

    float fitness_number = total_curr_assets - total_prev_assets;

    return fitness_number;
}

float Agent::getFitness() const
{
    float total_prev_assets = prev_assets.silver +
            prev_assets.gold * 3 +
            prev_assets.platinum * 6;

    float total_curr_assets = assets.silver +
            assets.gold * 3 +
            assets.platinum * 6;

    float fitness_number = total_curr_assets - total_prev_assets;

    return fitness_number;
}

bool Agent::assetsAvailable(const TradeGame::Bid &bid)
{
    bool tooMuch = false;
    switch (bid.buyingType)
    {
        case TradeGame::GOLD:
            if (assets.gold < bid.buyingVolume)
                tooMuch = true;
            break;
        case TradeGame::SILVER:
            if (assets.silver < bid.buyingVolume)
                tooMuch = true;
            break;
        case TradeGame::PLATINUM:
            if (assets.platinum < bid.buyingVolume)
                tooMuch = true;
            break;

        default:
            break;
    }

    return tooMuch;
}

}

