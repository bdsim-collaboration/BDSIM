/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSPlanckEngine_h
#define BDSPlanckEngine_h 1

#include "G4ios.hh" 
#include "globals.hh"
#include "Randomize.hh" 
#include "G4Version.hh"
#if G4VERSION_NUMBER < 1000
#include "G4VeLowEnergyLoss.hh"
#endif
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4OrderedTable.hh" 
#include "G4PhysicsTable.hh"
#include "G4PhysicsLogVector.hh"
 
class BDSPlanckEngine  
{ 
  public:
 
     BDSPlanckEngine(G4double aTemperature);
 
    ~BDSPlanckEngine();

     G4LorentzVector PerformPlanck();
     void SetTemperature(G4double aTemperature);

  protected:

  private:

  G4double PlanckSpectrum(G4double x);

private:
    G4double itsTemperature, kT;
    G4LorentzVector itsFourMom;

    G4double a,b,c,x1,x2,TotalArea,area1, area2, area3;

   const static G4int ntryMax = 10000000;

};


inline void BDSPlanckEngine::SetTemperature(G4double aTemperature)
{
 itsTemperature=aTemperature;
 kT=CLHEP::k_Boltzmann* itsTemperature;
}

inline G4double BDSPlanckEngine::PlanckSpectrum(G4double x)
{ return x*x/(exp(x)-1);}

#endif
