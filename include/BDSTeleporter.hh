#ifndef BDSTELEPORTER_H
#define BDSTELEPORTER_H

#include "BDSTeleporterStepper.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "BDSMagFieldMesh.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSBeamline.hh"

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
  BDSMagFieldMesh*            itsMagField;
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
}


#endif
