#ifndef BDSModularPhysicsList_h
#define BDSModularPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "BDSDebug.hh"
#include "G4VPhysicsConstructor.hh"
#include "BDSMuonPhysics.hh"
#include "BDSCutsAndLimits.hh"

#include <vector>

class BDSModularPhysicsList: public G4VModularPhysicsList
{
public:
  BDSModularPhysicsList();
  virtual ~BDSModularPhysicsList();
  void Print();
  virtual void SetCuts();

private:
  void SetParticleDefinition();
  void ConstructMinimumParticleSet();
  G4VPhysicsConstructor* _emPhysics;
  G4VPhysicsConstructor* _hadronicPhysics;
  G4VPhysicsConstructor* _muonPhysics;
  G4OpticalPhysics*      _opticalPhysics;
  G4VPhysicsConstructor* _decayPhysics;
  G4VPhysicsConstructor* _paramPhysics;
  G4VPhysicsConstructor* _synchRadPhysics;
  G4VPhysicsConstructor* _cutsAndLimits;
  std::vector<G4VPhysicsConstructor*> _constructors;
  void ParsePhysicsList();
  void ConfigurePhysics();
  void ConfigureOptical();
  void Register();
  G4String _physListName;

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

