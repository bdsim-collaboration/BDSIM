#ifndef BDSKICKER_H
#define BDSKICKER_H 

#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSKicker: public BDSMagnet
{
public:
  /// Constructor. "Type" represents whether it's a vertical or horizontal kicker.
  BDSKicker(G4String            name,
	    G4double            length,
	    G4double            bFieldIn,
	    G4double            kickAngle,
	    BDSMagnetType       type,
	    BDSBeamPipeInfo*    beamPipeInfo,
	    BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSKicker(){;};
  
protected:
  virtual void Build();

private:
  G4double bField;
  G4double kickAngle;

  virtual void BuildBPFieldAndStepper();
  virtual void BuildBeampipe(); // overload multipole base class method

};

#endif
