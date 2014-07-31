#ifndef BDSTeleporter_h
#define BDSTeleporter_h 1

#include "BDSTeleporterStepper.hh"
#include "G4VisAttributes.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "BDSMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
//#include "globals.hh"
//#include "BDSGlobalConstants.hh"
//#include "BDSDrift.hh"
#include "BDSBeamline.hh"
#include "parser/elementlist.h"

class BDSTeleporter: public BDSAcceleratorComponent
{
public:
  BDSTeleporter(G4String name,
		G4double length);  
  ~BDSTeleporter(){};

protected:
  G4ChordFinder*          itsChordFinder;
  G4FieldManager*         itsFieldManager;
  G4MagIntegratorStepper* itsStepper;
  BDSMagField*            itsMagField;
  G4Mag_UsualEqRhs*       itsEqRhs;

private:
  void CreateTeleporterLogicalVolume();
  void CreateBFieldAndStepper();
  void CreateFieldManager(G4MagIntegratorStepper* stepper,G4MagneticField* field);
  G4VisAttributes* SetVisAttributes();
};

void CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline);
void AddTeleporterToEndOfBeamline(ElementList* beamline_list);


#endif
