/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSComptonEngine_h
#define BDSComptonEngine_h 1

#include "G4ios.hh" 
#include "globals.hh"
#include "Randomize.hh" 
#include "G4Version.hh"

#if G4VERSION_NUMBER > 899
#include "G4VEnergyLossProcess.hh"
#else
#include "G4VeEnergyLoss.hh"
#endif

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4OrderedTable.hh" 
#include "G4PhysicsTable.hh"
#include "G4PhysicsLogVector.hh"
 
class BDSComptonEngine  
{ 
  public:
 
     BDSComptonEngine();

     BDSComptonEngine(G4LorentzVector InGam_FourVec, 
                      G4LorentzVector InEl_FourVec );
 
    ~BDSComptonEngine();

  void PerformCompton();
  void PerformHighEnergyCompton();
  void PerformHighEnergyCompton2();
  G4double ComptonDifferentialCrossSection(G4double costh=0, G4double gamma2=0);
  G4double PeakAmplitudeOfComptonDifferentialCrossSection(G4double gamma2=0);
  void SetIncomingPhoton4Vec(G4LorentzVector inGam);
  void SetIncomingElectron4Vec(G4LorentzVector inEl);

     G4LorentzVector GetScatteredElectron();
     G4LorentzVector GetScatteredGamma();

  protected:

  private:

private:
    G4LorentzVector itsScatteredEl;
    G4LorentzVector itsScatteredGam;
    G4LorentzVector itsIncomingEl;
    G4LorentzVector itsIncomingGam;

   static const G4int ntryMax = 10000000;

};

inline G4LorentzVector BDSComptonEngine::GetScatteredElectron()
{return itsScatteredEl;}

inline G4LorentzVector BDSComptonEngine::GetScatteredGamma()
{return itsScatteredGam;}


inline void BDSComptonEngine::SetIncomingPhoton4Vec(G4LorentzVector inGam)
{itsIncomingGam=inGam;
 if(itsIncomingEl.e()<CLHEP::electron_mass_c2)
      {G4Exception("BDSComptonEngine: Invalid Electron Energy", "-1", FatalException, "");}

}
inline void BDSComptonEngine::SetIncomingElectron4Vec(G4LorentzVector inEl)
{itsIncomingEl=inEl;}
#endif
