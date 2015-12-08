#ifndef BDSMUSPOILER_H
#define BDSMUSPOILER_H

#include "globals.hh"
#include "BDSMagnet.hh"
#include "BDSMuSpoilerMagField.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSMuSpoiler: public BDSMagnet
{
public:
  BDSMuSpoiler(G4String            name,
	       G4double            length,
	       G4double            bField,
	       BDSBeamPipeInfo*    beamPipeInfo,
	       BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSMuSpoiler();

private:
  G4double     itsBField;
  
  /// Override BDSMagnet function and do nothing so no field is
  /// attached to the beam pipe
  virtual void BuildBPFieldAndStepper();

  /// Override BDSMagnet outer magnetic field constructor so
  /// we can construct a mu spoiler field instead. Has to have
  /// same signature, but doesn't need the arguments so commented out.
  virtual void BuildOuterFieldManager(G4int    /*nPoles*/,
				      G4double /*poleField*/,
				      G4double /*phiOffset*/);
};

#endif
