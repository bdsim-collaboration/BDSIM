#ifndef BDSMUSPOILER_H
#define BDSMUSPOILER_H

#include "globals.hh"
#include "BDSMagnet.hh"
#include "BDSMuSpoilerMagField.hh"

struct BDSBeamPipeInfo;
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
  
  /// build the necessary field for muspoiler
  virtual void BuildBPFieldAndStepper();

  /// override multipole method to attach outer field
  virtual void BuildOuterVolume();

  // BDSMuSpoilerMagField* outerMagField;
  // G4FieldManager*       outerFieldMgr;
};

#endif
