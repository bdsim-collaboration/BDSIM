 
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

#include <vector>

class BDSPhysicsList: public G4VUserPhysicsList
{
public:
  BDSPhysicsList();
  virtual ~BDSPhysicsList();

public:

  void AddParameterisation();
  
  virtual void ConstructParticle();
  virtual void ConstructProcess();

  virtual void SetCuts();

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

  /// A vector of all the physics lists names
  std::vector<G4String> physicsListNames;

  G4Cerenkov*          theCerenkovProcess;
  G4Scintillation*     theScintillationProcess;
  G4OpAbsorption*      theAbsorptionProcess;
  G4OpRayleigh*        theRayleighScatteringProcess;
  G4OpMieHG*           theMieHGScatteringProcess;
  G4OpBoundaryProcess* theBoundaryProcess;

  /// physics lists
  // standard physics list
  G4VPhysicsConstructor* theReferenceHadronicPhysList;
  G4VPhysicsConstructor* theReferenceEmPhysList;
  // BDSIM physics lists
  G4VPhysicsConstructor* theBDSIMPhysList;
  // hadronic physics lists
  G4VPhysicsConstructor* theHadPhysList1;
  G4VPhysicsConstructor* theHadPhysList2;
};

#endif



