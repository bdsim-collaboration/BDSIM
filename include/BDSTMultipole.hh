/* BDSIM

19 May 2008 by Marchiori G.
17 Apr 2006 by Ilya Agapov

*/

#ifndef BDSTMultipole_h
#define BDSTMultipole_h 1

#include "globals.hh"
#include "BDSMultipole.hh"

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
    ~BDSTMultipole();

private:
  std::list<G4double> kn; // list of normal multipole strengths 1/Brho * Bn
		     // (NOT multiplied by multipole length)
  std::list<G4double> ks; // list of skew multipole strengths 1/Brho * Bsn
		     // (NOT multiplied by multipole length)
  G4int itsOrder;
  
  virtual void BuildBPFieldAndStepper();

  virtual void SetVisAttributes();
};

#endif
