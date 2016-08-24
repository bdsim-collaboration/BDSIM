#ifndef BDSTELEPORTER_H
#define BDSTELEPORTER_H

#include "BDSAcceleratorComponent.hh"

class BDSBeamline;
class BDSMagField;
class G4ChordFinder;
class G4FieldManager;
class G4MagInt_Driver;
class G4MagIntegratorStepper;
class G4Mag_UsualEqRhs;

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
  G4MagInt_Driver*        magIntDriver;

private:
  virtual void Build();
  virtual void BuildContainerLogicalVolume();
  /// define field and stepper
  void BuildBPFieldAndStepper();
  /// build and set field manager and chord finder
  void BuildBPFieldMgr();
};

namespace BDS {
  void CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline);
}


#endif
