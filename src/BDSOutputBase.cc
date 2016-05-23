#include "BDSOutputBase.hh"

#include "BDSGlobalConstants.hh"

BDSOutputBase::BDSOutputBase():eventNumber(0),outputFileNumber(-1)
{
  numberEventPerFile = BDSGlobalConstants::Instance()->NumberOfEventsPerNtuple();
}

void BDSOutputBase::Commit()
{
  Write();
  Close();
  Initialise();
}
