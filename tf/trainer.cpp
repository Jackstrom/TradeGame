#include "trainer.h"
#include <algorithm>
#include <iostream>
#include <time.h>

namespace TF
{

Trainer::Trainer(QObject *parent) :
    QObject(parent)
{
    init();
}

Trainer::Trainer(AgentVector agents, QObject *parent) :
    agents(agents),
    QObject(parent)
{
    init();
}

Trainer::~Trainer()
{
    AgentVector::iterator it;

    for (it = agents.begin(); it != agents.end(); it++)
    {
        if (*it)
            delete *it;
    }
}

void Trainer::init()
{
    for (unsigned int i = 0; i < N_AGENTS; i++)
    {
        Genome genome;
        genome.initRandomData();
        Agent *agent = new Agent(genome);
        agents.push_back(agent);
    }
}

void Trainer::breed()
{
    std::sort(agents.begin(), agents.end(), fitness_comparison());
    emit agentsChanged(agents);

    children.clear();

    AgentVector selected_agents = selection();

    AgentVector selected_agents_one;
    AgentVector selected_agents_two;

    for (unsigned int i = 0; i < selected_agents.size(); i++)
    {
        if (i % 2)
            selected_agents_one.push_back(selected_agents[i]);
        else
            selected_agents_two.push_back(selected_agents[i]);
    }

    unsigned int min_size = (selected_agents_one.size() < selected_agents_two.size()) ?
                selected_agents_one.size() : selected_agents_two.size();

    if (min_size != 0)
    {
        Agent *braut = selected_agents.at(0);
        for (unsigned int i = 0; i < min_size; i++)
        {
            Agent *child_one = crossover(*braut, *selected_agents_one[i], rand() % 2);
            Agent *child_two = crossover(*selected_agents_two[i], *selected_agents_one[i], rand() % 2);
//            Agent *child_three = crossover(*selected_agents_one[i], *selected_agents_two[i], rand() % 2);

//            if (child_three)
//                children.push_back(child_three);


            if (child_one && braut != selected_agents_one[i])
            {
                if (rand() % 100 < 10)
                {
                    Genome genome_one = child_one->getGenome();
                    mutate(genome_one);
                    child_one->setGenome(genome_one);
                }

                children.push_back(child_one);
            }

            if (child_two)
            {
                if (rand() % 100 > 80)
                {
                    Genome genome_two = child_two->getGenome();
                    mutate(genome_two);
                    child_two->setGenome(genome_two);
                }

                children.push_back(child_two);
            }

        }

        std::sort(children.begin(), children.end(), fitness_comparison());
        Agent *b = children.at(0);
        b->setName("Brautern");

        emit childrenChanged(children);
    }

    agents.clear();
    agents = children;
}

// TODO: Complete me
AgentVector Trainer::selection()
{
    AgentVector selected_agents;

    AgentVector::iterator it;

    std::sort(agents.begin(), agents.end(), fitness_comparison());

    unsigned int i = 0;
    for (it = agents.begin(); it != agents.end(); it++, i++)
    {
        Agent *agent = *it;
        if (agents.size() > N_AGENTS * 2)
        {
            if (agent->getFitness() > 100)
                selected_agents.push_back(agent);
        }
        else
            selected_agents.push_back(agent);

    }

    emit selectionChanged(selected_agents);

    return selected_agents;
}

Agent *Trainer::crossover(const Agent &parent_one,
                          const Agent &parent_two,
                          bool cross_back)
{
    Genome genome_one = parent_one.getGenome();
    Genome genome_two = parent_two.getGenome();

    if (!cross_back)
    {
        if (rand() % 2)
        {
            genome_one.trading_policy = genome_two.trading_policy;
            genome_one.sell_vol_silver = genome_two.sell_vol_silver;
        }

        genome_one.willingness = genome_two.willingness;
        genome_one.collecting_policy = genome_two.collecting_policy;
        genome_one.sell_policy = genome_two.sell_policy;
    }
    else
    {
        if (rand() % 2)
        {
            genome_one.buy_vol_gold = genome_two.buy_vol_gold;
            genome_one.sell_vol_platinum = genome_two.sell_vol_platinum;
        }

        genome_one.sell_vol_gold = genome_two.sell_vol_gold;
        genome_one.buy_vol_silver = genome_two.buy_vol_silver;
        genome_one.buy_vol_platinum = genome_two.buy_vol_platinum;
    }

    Agent *child;
    if (genome_two.collecting_policy == genome_two.sell_policy)
        child = 0; // retardation
    else
        child = new Agent(genome_two);

    return child;
}

void Trainer::mutate(Genome &genome)
{
    unsigned int position = rand() % 10;

    switch(position)
    {
        case 0:
            genome.willingness = rand() % 3;
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            genome.trading_policy = TradingPolicy(rand() % 3) ;
            break;
        case 4:
            genome.sell_vol_silver = rand() % 50;
            break;
        case 5:
            genome.sell_vol_gold = rand() % 50;
            break;
        case 6:
            genome.sell_vol_platinum = rand() % 50;
            break;
        case 7:
            genome.buy_vol_silver = rand() % 50;
            break;
        case 8:
            genome.buy_vol_gold = rand() % 50;
            break;
        case 9:
            genome.buy_vol_platinum = rand() % 50;
            break;
        default:
            break;
    }
}

void Trainer::inversion(Genome &genome,
                        unsigned int start,
                        unsigned int end)
{

}

}


