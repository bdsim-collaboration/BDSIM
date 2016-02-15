#ifndef BDSPRIMARYGENERATORACTION_H
#define BDSPRIMARYGENERATORACTION_H 

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class BDSBunch;
class G4Event;
class G4ParticleGun;

class BDSPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  BDSPrimaryGeneratorAction(BDSBunch*);    
  ~BDSPrimaryGeneratorAction();
  
public:
  virtual void GeneratePrimaries(G4Event*);
  
private:
  /// pointer a to G4 service class
  G4ParticleGun*   particleGun;	  
  /// weight of the primaries (currently always 1 and not possible to change it)
  G4double         weight;
  /// pointer to the particle distribution generator
  BDSBunch*        bdsBunch;
};

#endif


