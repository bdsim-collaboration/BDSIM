#ifndef BDSDECAPOLE_H
#define BDSDECAPOLE_H

#include "globals.hh"

#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

/**
 * @brief Decapole magnet.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSDecapole: public BDSMagnet
{
public:
  BDSDecapole(G4String            name,
	      G4double            length,
	      G4double            bQuadPrime,
	      BDSBeamPipeInfo*    beamPipeInfo,
	      BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSDecapole(){;};

protected:
  virtual void Build();

private:
  G4double itsBQuadPrime;

  virtual void BuildBPFieldAndStepper();
};

#endif
