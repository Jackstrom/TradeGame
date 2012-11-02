#ifndef BOT123_H
#define BOT123_H

#include "agent.h"
#include "types.h"
#include "intercom.h"
#include <QThread>
#include "genalgocomm.h"
#include <QSharedPointer>
#include <QMutex>

class TCommThread: public QThread {
private:
  static const float F_RESET = -1; //<0

  QSharedPointer<TGenAlgoComm> GenAlgoComm;
  QMutex MPhenotype, MFunc;

  Phenotype123 Phenotype;
  float ObjFunc;

  bool Running;
  QString FAgentName;

protected:
  void run();

public:
  TCommThread(QString AgentName);
  void Stop(void) { Running=false; /* [TODO: also reset semaphores] */ }

  Phenotype123 get_Phenotype(void);
  void set_ObjFunc(float F);
};


namespace TradeGame
{

class Bot123: public Agent
{
private:
  int TurnCount;
  ushort Willingness;
  TCollectingPolicy CPolicy;
  Bid CurrentBid;
  Assets MyAssets;
  QSharedPointer<TCommThread> CommThread;
  QString MyName;
  std::string MyNameS;

protected:
  //QString ReadSuffixFromFile(void);
  int GetAxIndex(TCollectingPolicy CPolicy, bool Buying);
  TCollectingPolicy NextPolicy(TCollectingPolicy p);
  int GetAsset(TCollectingPolicy Asset);
  TCollectingPolicy ChooseMetalForSale(TCollectingPolicy Purchase);

public:
  virtual bool start();

  virtual Bid inviteBid();

  virtual int selectBestMatch(const std::vector<Bid>& bids);
  virtual void addAssets(int dSilver, int dGold, int dPlatinum);

  virtual Assets getAssets() const;
  virtual void setAssets(const Assets& assets);
  virtual const std::string getName() const;

  Bot123(QString NameSuffix);
  virtual ~Bot123();
};

}

#endif // BOT123_H
