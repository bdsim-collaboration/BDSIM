/* BDSIM

19 May 2008 by Marchiori G.
17 Apr 2006 by Ilya Agapov

*/

#ifndef BDSTMULTIPOLE_H
#define BDSTMULTIPOLE_H

#include "BDSMagnet.hh"

#include "globals.hh"
#include "G4Material.hh"

#include <list>

struct BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSTMultipole: public BDSMagnet
{
public:
  BDSTMultipole(G4String            name,
		G4double            length,
		std::list<G4double> akn, // list of normal multipole strengths
		                    // (NOT multiplied by multipole length)
		std::list<G4double> aks, // list of skew multipole strengths
		                    // (NOT multiplied by multipole length)
		BDSBeamPipeInfo*    beamPipeInfo,
		BDSMagnetOuterInfo  magnetOuterInfo,
		BDSTiltOffset       tiltOffset = BDSTiltOffset());
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
};

#endif
