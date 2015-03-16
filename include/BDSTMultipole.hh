/* BDSIM

19 May 2008 by Marchiori G.
17 Apr 2006 by Ilya Agapov

*/

#ifndef BDSTMultipole_h
#define BDSTMultipole_h 1

#include "globals.hh"
#include "BDSMultipole.hh"
#include "G4Material.hh"
#include "BDSBeamPipeInfo.hh"

#include <list>

class BDSTMultipole :public BDSMultipole
{
public:
  BDSTMultipole(G4String aName, G4double aLength,
		G4double bpRad, G4double FeRad,
		G4double tilt, G4double outR,
		std::list<G4double> akn, // list of normal multipole strengths
		                    // (NOT multiplied by multipole length)
		std::list<G4double> aks, // list of skew multipole strengths
		                    // (NOT multiplied by multipole length)
		std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                G4String aTunnelMaterial = "",
		G4String aMaterial = "");
  BDSTMultipole(G4String            name,
		G4double            length,
		std::list<G4double> akn, // list of normal multipole strengths
		                    // (NOT multiplied by multipole length)
		std::list<G4double> aks, // list of skew multipole strengths
		                    // (NOT multiplied by multipole length)
		beamPipeInfo        beamPipeInfo,
		G4double            boxSize,
		G4String            outerMaterial="",
		G4String            tunnelMaterial="",
		G4double            tunnelRadius=0,
		G4double            tunnelOffsetX=0);
  ~BDSTMultipole(){;};
  
private:
  /// old and new constructor contents in temporary function to avoid replicating
  void CommonConstructor(std::list<G4double> akn, std::list<G4double> aks); 
  std::list<G4double> kn; // list of normal multipole strengths 1/Brho * Bn
		     // (NOT multiplied by multipole length)
  std::list<G4double> ks; // list of skew multipole strengths 1/Brho * Bsn
		     // (NOT multiplied by multipole length)
  G4int itsOrder;
  
  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();
  virtual void BuildOuterVolume();
};

#endif
