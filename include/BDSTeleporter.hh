#ifndef BDSTeleporter_h
#define BDSTeleporter_h 1

#include "BDSTeleporterStepper.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "BDSMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSBeamline.hh"
#include "parser/elementlist.h"

class BDSTeleporter: public BDSAcceleratorComponent
{
public:
  BDSTeleporter(G4String name,
		G4double length);  
  ~BDSTeleporter();

protected:
  G4ChordFinder*          itsChordFinder;
  G4FieldManager*         itsFieldManager;
  G4MagIntegratorStepper* itsStepper;
  BDSMagField*            itsMagField;
  G4Mag_UsualEqRhs*       itsEqRhs;

private:
  virtual void Build();
  virtual void BuildContainerLogicalVolume();
  /// define field and stepper
  void BuildBPFieldAndStepper();
  /// build and set field manager and chord finder
  void BuildBPFieldMgr(G4MagIntegratorStepper* stepper,G4MagneticField* field);
};

namespace BDS {
  void CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline);
  void AddTeleporterToEndOfBeamline(ElementList* beamline_list);
}


#endif
