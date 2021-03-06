 #ifndef AGENT_H
#define AGENT_H

#include <memory>
#include <vector>
#include <string>
#include "types.h"

namespace TradeGame
{

class Agent
{
public:

    virtual bool start() = 0;  //returns true if it wants to trade.

    /**
      * Agent is defined as seller.
      */
    virtual Bid inviteBid() = 0;

    /**
      * Returns the position in the vector.
      * Receiving agent is defined as buyer.
      * Return negative number to not partake.
      */
    virtual int selectBestMatch(const std::vector<Bid>& bids) = 0;
    virtual void addAssets(int dSilver, int dGold, int dPlatinum) = 0;

    virtual Assets getAssets() const = 0;
    virtual void setAssets(const Assets& assets) = 0;
    virtual const std::string getName() const = 0;

    virtual ~Agent(){}
};

}

#endif // AGENT_H
