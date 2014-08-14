#ifndef __BDS_CUTS_AND_LIMITS_HH_
#define __BDS_CUTS_AND_LIMITS_HH_


#include "G4UserSpecialCuts.hh"
#include "G4StepLimiter.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"


class BDSCutsAndLimits: public BDSXSBiasPhysics{
public:
  BDSCutsAndLimits();
  ~BDSCutsAndLimits();
  void ConstructProcess();
  void ConstructParticle();
};
#endif
