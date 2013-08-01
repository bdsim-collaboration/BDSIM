
#ifndef mySextupole_h
#define mySextupole_h 1

#include "mySextStepper.hh"
#include "G4ChordFinder.hh"
#include "G4VisAttributes.hh"
#include "BDSMultipole.hh"
#include "myAcceleratorComponent.hh"
#include "G4FieldManager.hh"
#include "G4Box.hh"
#include "G4Box.hh"

class mySextupole :public myAcceleratorComponent
{

public:
  //	Constructor taking the id of the dipole, the length
  //	(meter) and curvature (1/meter) of the dipole geometry,
  //	and the vertical magnetic field in Tesla.
  mySextupole (G4String& id, G4double len, G4double bpRad,G4double bDGrad);

  ~mySextupole();

  G4VisAttributes* SetVisAttributes();

  void SextLogicalVolume();
  void BuildOuterFieldManager();

protected:
 
private:
  // field related objects:
  mySextStepper* itsStepper;
  G4ChordFinder* itsChordFinder;
  G4MagneticField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;
  G4FieldManager* itsBPFieldMgr;
  G4Box* itsBox;

  //G4FieldManager* itsCoarseFieldMgr;
  //G4ChordFinder* itsCoarseChordFinder;

};


#endif
