#ifndef BDSSectorBend_h
#define BDSSectorBend_h 

#include "globals.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMultipole.hh"
#include "BDSTunnelInfo.hh"

#include <list>

class BDSSectorBend :public BDSMultipole
{
public:
  BDSSectorBend(G4String           nameIn,
		G4double           lengthIn,
		G4double           angleIn,
		G4double           bFieldIn,
		G4double           bGradIn,
		BDSBeamPipeInfo    beamPipeInfo,
		BDSMagnetOuterInfo magnetOuterInfo,
		BDSTunnelInfo      tunnelInfo);
  ~BDSSectorBend(){;};

  virtual G4double GetChordLength();

private:
  G4double itsBField;
  G4double itsBGrad;
  
  /// chord length in [m]
  G4double itsChordLength;

  /// normal vectors for faces when preparing solids
  G4ThreeVector inputface;
  G4ThreeVector outputface;
  
  /// orientation of shifts - depends on angle - calculations use absolute value of angle for safety
  G4int orientation;

  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildMarkerLogicalVolume();
  virtual void BuildBeampipe();
};

#endif
