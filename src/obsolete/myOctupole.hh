
#ifndef myOctupole_h
#define myOctupole_h 1

#include "myOctStepper.hh"
#include "G4ChordFinder.hh"
#include "G4VisAttributes.hh"
#include "BDSMultipole.hh"
#include "myAcceleratorComponent.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
class myOctupole :public myAcceleratorComponent
{

public:
  //	Constructor taking the id of the dipole, the length
  //	(meter) and curvature (1/meter) of the dipole geometry,
  //	and the vertical magnetic field in Tesla.
  myOctupole (G4String& id, G4double len, G4double bpRad,G4double bDGrad);

  ~myOctupole();

  void OctLogicalVolume();
  void BuildOuterFieldManager();

  G4VisAttributes* SetVisAttributes();

protected:
 
private:
  // field related objects:
  myOctStepper* itsStepper;
  G4ChordFinder* itsChordFinder;
  G4MagneticField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;
  G4FieldManager* itsBPFieldMgr;
  G4Box* itsBox;

  //  G4FieldManager* itsCoarseFieldMgr;
  //  G4ChordFinder* itsCoarseChordFinder;

};


#endif
