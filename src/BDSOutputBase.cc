#include "BDSOutputBase.hh"

#include "BDSGlobalConstants.hh"

BDSOutputBase::BDSOutputBase():eventNumber(0),outputFileNumber(-1)
{
  numberEventPerFile = BDSGlobalConstants::Instance()->GetNumberOfEventsPerNtuple();
}

void BDSOutputBase::Commit()
{
  Write();
  Close();
  Initialise();
}
