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

BDSOutputROOTEventHistograms* BDSOutputBase::GetEventAnalysis()
{
  G4cout << "BDSOutputBase::GetEventAnalysis()" << G4endl;
  return nullptr;
}

BDSOutputROOTEventHistograms* BDSOutputBase::GetRunAnalysis()
{
  G4cout << "BDSOutputBase::GetRunAnalysis()" << G4endl;
  return nullptr;
}