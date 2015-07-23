/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added GABs SynchGen code
*/


#ifndef BDSPrimaryGeneratorAction_h
#define BDSPrimaryGeneratorAction_h 

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


