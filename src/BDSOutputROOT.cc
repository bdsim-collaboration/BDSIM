#include "BDSOutputROOT.hh"
#include "BDSOutputROOTBase.hh"

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSSamplerBase.hh"
#include "BDSTrajectory.hh"
#include "BDSUtilities.hh"
#include "BDSHistogram.hh"

#include <string>

BDSOutputROOT::BDSOutputROOT()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format ROOT"<<G4endl;
#endif
  Init(); // activate the output
}

BDSOutputROOT::~BDSOutputROOT()
{;}

TTree* BDSOutputROOT::BuildSamplerTree(G4String name)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSOutputROOTBase::BuildSamplerTreeBasic(name);
}
