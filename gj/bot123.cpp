#include "bot123.h"
#include <QCoreApplication>
#include <QFile>
#include <stdlib.h>

using namespace TradeGame;
using namespace std;

Bot123::Bot123(QString NameSuffix) : TurnCount(0)
{
  QString MyName="123bot_"+NameSuffix;
  MyNameS=MyName.toStdString();
  CommThread = QSharedPointer<TCommThread>(new TCommThread(MyName));
  CommThread->start();
}

Bot123::~Bot123()
{
  CommThread->Stop();
  CommThread->terminate();
}

bool Bot123::start()
{
  TurnCount++;

  qDebug(QString("start: TurnCount=%1, Willingness=%2").arg(TurnCount).arg(Willingness).toAscii());
  if (TurnCount > Willingness) {
    TurnCount=0;
    return true;
  }

  return false;
}

#define SCALE(Expr, Factor) Expr=Expr*Factor

Bid Bot123::inviteBid()
{
  Bid Result;

  Phenotype123 ph = CommThread->get_Phenotype();
  Willingness = ph.Willingness % 5;

  switch (ph.CPolicy) {
  case (cpGold):
    Result.buyingType=GOLD;
    break;
  case (cpSilver):
    Result.buyingType=SILVER;
    break;
  case (cpPlatinum):
    Result.buyingType=PLATINUM;
    break;
  }

  //********************************************************

  CPolicy = (TCollectingPolicy)ph.CPolicy;
  TCollectingPolicy SellPolicy=ChooseMetalForSale(CPolicy);

  Result.buyingVolume = ph.Assets[GetAxIndex(CPolicy, true)];
  Result.sellingVolume = ph.Assets[GetAxIndex(SellPolicy, false)];
  //%10
  const int MOD=10;
  const int SELL_FACTOR=2;

  int iAsset = GetAsset(SellPolicy);
  if (Result.sellingVolume > iAsset) {

    Result.sellingVolume %= MOD;

    if (iAsset!=0)
      Result.sellingVolume %= iAsset; else
      Result.sellingVolume=0;
  }


  if (Result.buyingVolume > Result.sellingVolume*SELL_FACTOR) {

    Result.buyingVolume %= MOD;

    if (Result.sellingVolume!=0)
      Result.buyingVolume %= Result.sellingVolume*SELL_FACTOR;
  }

  //********************************************************

  switch (ph.TPolicy) {
  case (tpGreedy):
    SCALE(Result.buyingVolume, 12/10);
    SCALE(Result.sellingVolume, 9/10);
    break;
  case (tpNeutral):
    //do nothing
    /*try to balance offer in terms of original asset value*/
    break;
  case (tpStealthy):
    SCALE(Result.buyingVolume, 8/10);
    SCALE(Result.sellingVolume, 11/10);
    break;
  }

  CurrentBid=Result;
  return Result;
}

int Bot123::selectBestMatch(const std::vector<Bid> &bids)
{
  uint L=bids.size();

  int best_ix=rand()*(int)L / RAND_MAX;
  float best_d=100000000;

  for (uint i=0; i<L; i++) {
    if (bids[i].sellingType==CurrentBid.sellingType &&
        bids[i].buyingType==CurrentBid.buyingType) {

      float d=0; //"deviation"
      d += CurrentBid.sellingVolume - bids[i].buyingVolume;
      d -= CurrentBid.buyingVolume - bids[i].sellingVolume;

      qDebug(QString("d = %1").arg(d).toAscii());
      if (d<best_d) {
        best_ix=(int)i;
        best_d=d;
      }
    }
  }

  qDebug(QString("selectBestMatch: %1/%2").arg(best_ix).arg(bids.size()).toAscii());
  return best_ix;
}

void Bot123::addAssets(int dSilver, int dGold, int dPlatinum)
{
  const float FACTOR=2;
  float arFactors[3]={1, 1, 1};

  switch (CPolicy) {
  case (cpSilver):
    arFactors[0]=FACTOR;
    break;
  case (cpGold):
    arFactors[1]=FACTOR;
    break;
  case (cpPlatinum):
    arFactors[2]=FACTOR;
    break;
  }

  float f = dSilver*arFactors[0] + dGold*arFactors[1] + dPlatinum*arFactors[2];
  CommThread->set_ObjFunc(f);

  MyAssets.silver+=dSilver;
  MyAssets.gold+=dGold;
  MyAssets.platinum+=dPlatinum;
}

Assets Bot123::getAssets() const
{
  return MyAssets;
}

void Bot123::setAssets(const Assets &assets)
{
  MyAssets=assets;
}

const std::string Bot123::getName() const
{
  return MyNameS;
}

TCommThread::TCommThread(QString AgentName) : ObjFunc(F_RESET), Running(true),
  FAgentName(AgentName)
{
  GenAlgoComm = QSharedPointer<TGenAlgoComm>(new TGenAlgoComm(AgentName));
}

void TCommThread::run()
{
  while (Running && (! GenAlgoComm->AttachToMemory()))
    msleep(500);
  qDebug(QString("%1: attached to shared memory").arg(FAgentName).toAscii());

  while (Running) {
    qDebug("--1");

    Phenotype123 ph = GenAlgoComm->ReadDNA();

    MPhenotype.lock();
    Phenotype=ph;
    MPhenotype.unlock();

    qDebug("--2");
    while (ObjFunc<0)
      msleep(10); //wait for objective function

    MFunc.lock();
    float F=ObjFunc;
    ObjFunc = F_RESET;
    MFunc.unlock();

    qDebug("--3");
    GenAlgoComm->WriteObjFunc(F);
  }
}

Phenotype123 TCommThread::get_Phenotype(void) {
  MPhenotype.lock();
  Phenotype123 Result=Phenotype;
  MPhenotype.unlock();
  return Result;
}

void TCommThread::set_ObjFunc(float F) {
  MFunc.lock();
  ObjFunc=F;
  MFunc.unlock();
}

int TradeGame::Bot123::GetAxIndex(TCollectingPolicy CPolicy, bool Buying)
{
  int Result=0;

  switch (CPolicy) {
  case (cpSilver):
    Result=0;
    break;
  case (cpGold):
    Result=1;
    break;
  case (cpPlatinum):
    Result=2;
    break;
  } //0..2

  if (Buying)
    Result+=3; //3..5

  return Result;
}

TCollectingPolicy TradeGame::Bot123::ChooseMetalForSale(TCollectingPolicy Purchase)
{
  TCollectingPolicy Result = NextPolicy(Purchase);

  if (GetAsset(Result)==0)
    Result = NextPolicy(Result); //if also zero, then no choice - try to trade for nothing

  return Result;
}

TCollectingPolicy TradeGame::Bot123::NextPolicy(TCollectingPolicy p)
{
  switch (p) {

  case (cpSilver):
    return cpGold;
  case (cpGold):
    return cpPlatinum;
  case (cpPlatinum):
    return cpSilver;
  }
}

int TradeGame::Bot123::GetAsset(TCollectingPolicy Asset)
{
  switch (Asset) {

  case (cpSilver):
    return MyAssets.silver;
  case (cpGold):
    return MyAssets.gold;
  case (cpPlatinum):
    return MyAssets.platinum;
  }
}

/*QString TradeGame::Bot123::ReadSuffixFromFile()
{
  QFile ConfigFile(QCoreApplication::applicationFilePath() + ".conf");
  if (ConfigFile.open(QIODevice::ReadOnly))
    return QString(ConfigFile.readLine()); else {

    qWarning("Could not open config file");
    return "";
  }
}
*/
