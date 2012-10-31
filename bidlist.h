#ifndef BIDLIST_H
#define BIDLIST_H

#include <map>
#include <vector>
#include "agent.h"

class BidList
{
public:
    class Iterator;
    friend class BidList::Iterator;
    BidList();

    void create(std::map<unsigned int, TradeGame::Agent*> agents);
    BidList::Iterator iterate();

    class Iterator
    {
    public:
        Iterator(BidList& parent);
        bool hasNext() const;
        unsigned int next();
        void remove(unsigned int seller, unsigned int buyer);

    private:
        BidList& parent;
        unsigned int current;
    };

private:
    std::vector<unsigned int> list;
};

#endif // BIDLIST_H
