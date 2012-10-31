#include "bidlist.h"
#include <algorithm>

#include <assert.h>

BidList::BidList()
{
}

void BidList::create(std::map<unsigned int,TradeGame::Agent*> agents)
{
    list.clear();
    for(std::map<unsigned int,TradeGame::Agent*>::iterator it=agents.begin(); it!=agents.end(); it++)
    {
        bool trading = it->second->start();
        if(trading)
            list.push_back(it->first);

    }
    std::random_shuffle(list.begin(), list.end());
}

BidList::Iterator BidList::iterate()
{
    BidList::Iterator iter(*this);
    return iter;
}


//***************Iterator***************//
BidList::Iterator::Iterator(BidList& parent) :
    parent(parent),
    current(0)
{
}

bool BidList::Iterator::hasNext() const
{
    return current < parent.list.size();// && !ended;
}

unsigned int BidList::Iterator::next()
{
    return parent.list.at(current++);
}

void BidList::Iterator::remove(unsigned int seller, unsigned int buyer)
{
    bool runCorrections = true;
    unsigned int newAssigned;
    if(parent.list.size() > current+2)
    {
        unsigned int assignedCurrent = parent.list.at(current);
        unsigned int assignedNext = parent.list.at(current+1);
        unsigned int assignedNextNext = parent.list.at(current+2);
        if(assignedCurrent != seller && assignedCurrent != buyer)
        {
            newAssigned = assignedCurrent;
        }
        else if(assignedNext != seller && assignedNext != buyer)
        {
            newAssigned = assignedNext;
        }
        else if(assignedNextNext != seller && assignedNextNext != buyer)
        {
            newAssigned = assignedNextNext;
        }
        else
            assert(false);
    }
    else
    {
        runCorrections = false;
    }
    for(std::vector<unsigned int>::iterator it=parent.list.begin(); it!=parent.list.end(); it++)
    {
        if(*it == seller || *it == buyer)
        {
            it = parent.list.erase(it);
            it--;
        }
    }
    if(runCorrections)
    {
        for(std::vector<unsigned int>::iterator it=parent.list.begin(); it!=parent.list.end(); it++)
        {
        }
        for(unsigned int i=0; i<parent.list.size(); i++)
        {
            if(parent.list.at(i) == newAssigned)
            {
                current = i;
                return;
            }
        }
    }
}
