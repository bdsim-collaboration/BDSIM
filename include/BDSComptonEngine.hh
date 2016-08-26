#ifndef BDSCOMPTONENGINE_H
#define BDSCOMPTONENGINE_H

#include "G4ios.hh" 
#include "globals.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "CLHEP/Units/PhysicalConstants.h"

/**
 * @brief Engine to calcualte product of compton scattering process.
 *
 * @author Grahame Blair.
 */

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
