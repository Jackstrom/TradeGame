#ifndef INTERCOM_H
#define INTERCOM_H

#include <QtGlobal>
#include <vector>
#include <QSharedMemory>
#include <QSystemSemaphore>

//typedef std::vector<char> TDNA_RawString;


enum TCollectingPolicy { cpSilver, cpPlatinum, cpGold };

enum TTradingPolicy { tpGreedy, tpNeutral, tpStealthy };

namespace phenotype123 {

const uchar N_COLL_POLICIES=3;
const uchar N_TRADE_POLICIES=3;

const uchar N_ASSETS=6;

}

struct Phenotype123 {
  ushort Willingness;
  uchar CPolicy, TPolicy;
  uint Assets[phenotype123::N_ASSETS];
};

class TProducerConsumer {
private:
  QSystemSemaphore* Free, * Used;
public:
  TProducerConsumer(QString SemaphoreName);
  ~TProducerConsumer(void);

  void ProducePre(void) { Free->acquire(); }
  void ProducePost(void) { Used->release(); }
  void ConsumePre(void) { Used->acquire(); }
  void ConsumePost(void) { Free->release(); }
};

class TTwoSideComm {
private:
  QString SemDNA_Name, SemFunc_Name;

  TProducerConsumer* pcDNA, * pcFunc; //semaphores for accessing DNA and ObjFunc
  QSharedMemory* ShmDNA, * ShmFunc;

  int DNA_Size;

public:
  TTwoSideComm(QString AgentName);
  ~TTwoSideComm(void);

  void CreateMemory(void); //DNA unit
  bool AttachToMemory(void); //Agent

  void WriteDNA(Phenotype123 Phenotype);
  Phenotype123 ReadDNA(void);
  void WriteObjFunc(float F);
  float ReadObjFunc(void);
};

#endif // INTERCOM_H
