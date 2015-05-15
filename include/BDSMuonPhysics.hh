#ifndef __BDS_MUON_PHYSICS_HH_
#define __BDS_MUON_PHYSICS_HH_


#include "BDSXSBiasPhysics.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "BDSVProcess.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4AnnihiToMuPair.hh"
#include "G4eeToHadrons.hh"

class BDSMuonPhysics: public BDSXSBiasPhysics{
public:
  BDSMuonPhysics();
  ~BDSMuonPhysics();
  void ConstructProcess();
  void ConstructParticle();
  
private:
  BDSXSBias* WrapXSBias(G4GammaConversionToMuons* proc, G4double efactor);
  BDSXSBias* WrapXSBias(G4AnnihiToMuPair* proc, G4double efactor);
  BDSXSBias* WrapXSBias(G4eeToHadrons* proc, G4double efactor);
};
#endif
