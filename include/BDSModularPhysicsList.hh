#ifndef BDSModularPhysicsList_h
#define BDSModularPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "G4VPhysicsConstructor.hh"
#include "BDSCutsAndLimits.hh"
#include "BDSGlobalConstants.hh"

#include <vector>


class BDSModularPhysicsList: public G4VModularPhysicsList
{
public:
  BDSModularPhysicsList();
  virtual ~BDSModularPhysicsList();
  void Print();
  virtual void SetCuts();

private:
  G4bool verbose;
#ifdef BDSDEBUG 
  bool debug = true;
#else 
  bool debug = false;
#endif
  
  void SetParticleDefinition();
  void ConstructMinimumParticleSet();
  G4VPhysicsConstructor* emPhysics;
  G4VPhysicsConstructor* hadronicPhysics;
  G4VPhysicsConstructor* muonPhysics;
  G4OpticalPhysics*      opticalPhysics;
  G4VPhysicsConstructor* decayPhysics;
  G4VPhysicsConstructor* paramPhysics;
  G4VPhysicsConstructor* synchRadPhysics;
  G4VPhysicsConstructor* cutsAndLimits;
  std::vector<G4VPhysicsConstructor*> constructors;
  void ParsePhysicsList();
  void ConfigurePhysics();
  void ConfigureOptical();
  void Register();
  G4String physListName;

  /// Keep a local reference to global constants to avoid getting it all the time
  BDSGlobalConstants* globals;

  //Physics constructor loaders.
  void LoadEm();			  
  void LoadEmLow();			  
  void LoadParameterisationPhysics();  
  void LoadHadronic();		  
  void LoadHadronicHP();		  
  void LoadSynchRad();
  void LoadMuon();						  
  void LoadOptical();		  
  void LoadDecay();			  
  void LoadCutsAndLimits();			  
};

#endif

