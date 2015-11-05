#ifndef BDSMULTIPOLE_H
#define BDSMULTIPOLE_H

#include "BDSMagnet.hh"

#include "globals.hh"
#include "G4Material.hh"

#include <list>

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSMultipole: public BDSMagnet
{
public:
  BDSMultipole(G4String             name,
	       G4double             length,
	       std::list<G4double>  akn, // list of normal multipole strengths
	                                 // (NOT multiplied by multipole length)
	       std::list<G4double>  aks, // list of skew multipole strengths
		                         // (NOT multiplied by multipole length)
	       BDSBeamPipeInfo*     beamPipeInfo,
	       BDSMagnetOuterInfo*  magnetOuterInfo);
  ~BDSMultipole(){;};
  
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
