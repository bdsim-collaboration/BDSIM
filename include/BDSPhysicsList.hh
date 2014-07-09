 
/** BDSIM, v0.1   

Last modified 01.02.2006 by Ilya Agapov

**/


#ifndef BDSPhysicsList_h
#define BDSPhysicsList_h

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4CoulombScattering.hh"

class BDSPhysicsList: public G4VUserPhysicsList
{
public:
  BDSPhysicsList();
  virtual ~BDSPhysicsList();

public:

 void AddParallelWorldName(G4String& pname)
         {paraWorldName.push_back(pname);}


  void AddScoringProcess();

  void AddParameterisation();
  
  void ConstructParticle();
  void ConstructProcess();

  void SetCuts();

  // construct particular physics list processes

  void ConstructEM();
  void ConstructEMSingleScatter();
  void ConstructEMMisc();
  void ConstructMultipleScattering();
  void ConstructCoulombScattering();

  void ConstructMuon();

  void ConstructDecay();

  void ConstructOptical();

  void ConstructMerlin();

  void ConstructEM_Low_Energy();

  void ConstructLaserWire();

  void ConstructHadronic();

  void ConstructSR();
  
private:
  G4bool verbose;

  std::vector<G4String>  paraWorldName;

    G4Cerenkov*          theCerenkovProcess;
    G4Scintillation*     theScintillationProcess;
    G4OpAbsorption*      theAbsorptionProcess;
    G4OpRayleigh*        theRayleighScatteringProcess;
    G4OpMieHG*           theMieHGScatteringProcess;
    G4OpBoundaryProcess* theBoundaryProcess;
};

#endif



