#pragma once

#include <time.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "agent.h"
#include "types.h"

namespace TF
{

enum TradingPolicy { Greedy, Neutral, Stealthy };

class Agent;
typedef std::vector<Agent*> AgentVector;

class Genome
{
public:
    Genome(){}
    Genome(unsigned int w,
           TradeGame::AssetType cp,
           TradeGame::AssetType sp,
           TradingPolicy p,
           unsigned int svs,
           unsigned int svg,
           unsigned int svp,
           unsigned int bvs,
           unsigned int bvg,
           unsigned int bvp) :
        willingness(w),
        collecting_policy(cp),
        sell_policy(sp),
        trading_policy(p),
        sell_vol_silver(svs),
        sell_vol_gold(svg),
        sell_vol_platinum(svp),
        buy_vol_silver(bvs),
        buy_vol_gold(bvg),
        buy_vol_platinum(bvp)
    {}

    std::string toString() const
    {
        std::string cp;
        switch(collecting_policy)
        {
            case TradeGame::GOLD:
                cp = "G";
                break;
            case TradeGame::SILVER:
                cp = "S";
                break;
            case TradeGame::PLATINUM:
                cp = "P";
                break;
            default:
                cp = "ERR";
        }

        std::string sp;
        switch(sell_policy)
        {
            case TradeGame::GOLD:
                sp = "G";
                break;
            case TradeGame::SILVER:
                sp = "S";
                break;
            case TradeGame::PLATINUM:
                sp = "P";
                break;
            default:
                sp = "ERR";
        }

        std::string tp;
        switch(trading_policy)
        {
            case Greedy:
                tp = "Gr";
                break;

            case Neutral:
                tp = "Ne";
                break;

            case Stealthy:
                tp = "St";
                break;

            default:
                tp = "ERR";
        }

        std::stringstream ss;
        ss << "["  << willingness << "]";
        ss << "["  << cp << "]";
        ss << "["  << sp << "]";
        ss << "["  << tp << "]";
        ss << "["  << sell_vol_silver << "]";
        ss << "["  << sell_vol_gold << "]";
        ss << "["  << sell_vol_platinum << "]";
        ss << "["  << buy_vol_silver << "]";
        ss << "["  << buy_vol_gold << "]";
        ss << "["  << buy_vol_platinum << "]";

        return ss.str();
    }

    void initRandomData()
    {
        willingness = rand() % 4;
        collecting_policy = TradeGame::AssetType(rand() % 3);
        sell_policy = TradeGame::AssetType(rand() % 3);
        trading_policy = TradingPolicy(rand() % 3);

        sell_vol_silver = static_cast<int>(rand() % 50) + 1;
        sell_vol_gold = static_cast<int>(rand() % 50) + 1;
        sell_vol_platinum = static_cast<int>(rand() % 50) + 1;

        buy_vol_silver = static_cast<int>(rand() % 50) + 1;
        buy_vol_gold = static_cast<int>(rand() % 50) + 1;
        buy_vol_platinum = static_cast<int>(rand() % 50) + 1;
    }

    unsigned int willingness;
    TradeGame::AssetType collecting_policy;
    TradeGame::AssetType sell_policy;
    TradingPolicy trading_policy;
    unsigned int sell_vol_silver;
    unsigned int sell_vol_gold;
    unsigned int sell_vol_platinum;
    unsigned int buy_vol_silver;
    unsigned int buy_vol_gold;
    unsigned int buy_vol_platinum;
};

class Agent : public TradeGame::Agent
{

public:
    Agent();
    Agent(const Genome &genome);
    Agent(unsigned int n_default_genome);

    void setGenome(const Genome &genome);
    Genome getGenome() const;

    int selectBestMatch(const std::vector<TradeGame::Bid> &bids);
    TradeGame::Bid inviteBid();

    void addAssets(int dSilver, int dGold, int dPlatinum);
    void setAssets(const TradeGame::Assets &assets);
    TradeGame::Assets getAssets() const;
    const std::string getName() const;
    void setName(const std::string &name);

    unsigned int getId() const;

    std::string generateName();

    bool start();
    float getFitness() const;
    float getFitness(TradeGame::Assets &curr_assets,
                     TradeGame::Assets &previous_assets) const;

private:
    bool ready;
    unsigned int id;
    std::string name;

    TradeGame::Assets assets;
    TradeGame::Assets prev_assets;

    Genome genome;

    void init();
    TradeGame::Bid createBid();

    bool assetsAvailable(const TradeGame::Bid &bid);
};

}
