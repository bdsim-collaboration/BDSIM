#include "BDSOutputBase.hh"

#include "BDSGlobalConstants.hh"

BDSOutputBase::BDSOutputBase():eventNumber(0),outputFileNumber(0)
{
  numberEventPerFile = BDSGlobalConstants::Instance()->GetNumberOfEventsPerNtuple();
}
