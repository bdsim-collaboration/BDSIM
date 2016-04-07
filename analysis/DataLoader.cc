#include "DataLoader.hh"

ClassImp(DataLoader);

DataLoader::DataLoader()
{
  this->CommonCtor();
}

DataLoader::DataLoader(Config &config)
{
  this->CommonCtor();
}

void DataLoader::CommonCtor()
{
  opt = new BDSOutputROOTEventOptions();
  mod = new BDSOutputROOTEventModel();
  evt = new Event();
}

DataLoader::~DataLoader()
{
  delete opt;
  delete mod;
  delete evt;
}

BDSOutputROOTEventOptions* DataLoader::GetOptions()
{
  return opt;
}

BDSOutputROOTEventModel* DataLoader::GetModel()
{
  return mod;
}

Event* DataLoader::GetEvent()
{
  return evt;
}