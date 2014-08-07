#ifndef BDSAwakePhysicsList_h
#define BDSAwakePhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "BDSDebug.hh"

class BDSAwakePhysicsList: public G4VModularPhysicsList
{
public:
  BDSAwakePhysicsList();
  virtual ~BDSAwakePhysicsList();

  virtual void SetCuts();
private:
  void SetParticleDefinition();
  G4OpticalPhysics* _opticalPhysics;
  G4EmPenelopePhysics* _emPhysics;
  void ConstructPhysics();
  void ConfigurePhysics();
  void Register();
};

#endif

