#ifndef BDSRFCAVITY_H
#define BDSRFCAVITY_H

#include "BDSMagnet.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4UniformElectricField.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSRfCavity: public BDSMagnet
{
public:
  BDSRfCavity(G4String            name,
	      G4double            length,
	      G4double            grad,
	      BDSBeamPipeInfo*    beamPipeInfoIn,
	      BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSRfCavity();
  
private:
  virtual void BuildBPFieldAndStepper();

  /// Override this function from BDSMagnet to prevent it from
  /// overwriting the custom field manager here that has an E field
  /// instead of the B field that all other derived classes of BDSMagnet
  /// have.
  virtual void BuildBPFieldMgr();
  
  G4double gradient; // longitudinal E field grad in MV / m

  // field related objects:
  G4UniformElectricField* eField;
  G4EqMagElectricField*   equation;
  G4MagInt_Driver*        intgrDriver;
};

#endif
