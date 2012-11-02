#include "intercom.h"

TTwoSideComm::TTwoSideComm(QString AgentName)
{
  const QString SEM="semaphore:";
  const QString SHM="shared:";

  const QString PREFIX="org.nuc.ai.";
  QString NameDNA = PREFIX+AgentName+".dna";
  QString NameFunc = PREFIX+AgentName+".func";

  pcDNA = new TProducerConsumer(SEM+NameDNA);
  pcFunc = new TProducerConsumer(SEM+NameFunc);

  /*//write
  SemDNA->acquire();
  ShmDNA->data();
  SemDNA->release();*/

  ShmDNA = new QSharedMemory(SHM+NameDNA);
  ShmFunc = new QSharedMemory(SHM+NameFunc);
}

TTwoSideComm::~TTwoSideComm(void)
{
  delete ShmDNA;
  delete ShmFunc;

  delete pcDNA;
  delete pcFunc;
}



void TTwoSideComm::CreateMemory(void)
{
  ShmDNA->create(sizeof(Phenotype123));
  ShmFunc->create(sizeof(float));
}

bool TTwoSideComm::AttachToMemory(void)
{
  return (ShmDNA->attach() && ShmFunc->attach());
}

void TTwoSideComm::WriteDNA(Phenotype123 Phenotype)
{
  pcDNA->ProducePre();

  ShmDNA->lock();

  char* dest = reinterpret_cast<char*>(ShmDNA->data());
  if (dest==NULL)
    dest=new char[sizeof(Phenotype)];

  char* src = reinterpret_cast<char*>(&Phenotype);

  memcpy(dest, src, sizeof(Phenotype));
  ShmDNA->unlock();

  pcDNA->ProducePost();
  qDebug("DNA produced");
}

Phenotype123 TTwoSideComm::ReadDNA(void)
{
  Phenotype123 Result;
  pcDNA->ConsumePre();

  ShmDNA->lock();
  if (ShmDNA->data() != NULL)
    memcpy(&Result, ShmDNA->data(), sizeof(Result));
  ShmDNA->unlock();

  pcDNA->ConsumePost();
  qDebug("DNA consumed");

  return Result;
}

void TTwoSideComm::WriteObjFunc(float F)
{
  pcFunc->ProducePre();

  ShmFunc->lock();

  memcpy(ShmFunc->data(), &F, sizeof(F));
  ShmFunc->unlock();

  pcFunc->ProducePost();
  qDebug("Func produced");
}

float TTwoSideComm::ReadObjFunc(void)
{
  float Result;
  pcFunc->ConsumePre();

  ShmFunc->lock();

  char* src = reinterpret_cast<char*>(ShmFunc->data());
  if (src==NULL)
    return 0; //Shm connection not established

  memcpy(&Result, src, sizeof(Result));

  //Result=*reinterpret_cast<float*>(ShmFunc->data());
  ShmFunc->unlock();

  pcFunc->ConsumePost();
  qDebug("Func consumed");

  return Result;
}

TProducerConsumer::TProducerConsumer(QString SemaphoreName)
{
  Free = new QSystemSemaphore(SemaphoreName+".free", 1, QSystemSemaphore::Create);
  Used = new QSystemSemaphore(SemaphoreName+".used", 0, QSystemSemaphore::Create);
}

TProducerConsumer::~TProducerConsumer() {
  delete Free;
  delete Used;
}
