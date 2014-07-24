#include "globals.hh" //G4 global constants & types
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSTeleporter.hh"

#include "BDSMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSTeleporterStepper.hh"

#include "parser/element.h"
#include "parser/elementlist.h"
#include "parser/enums.h"

#include "G4ThreeVector.hh"

void BDSTeleporter::BuildBpFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField = new BDSMagField(); //Zero magnetic field.
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  itsStepper  = new BDSTeleporterStepper(itsEqRhs);
}

void AddTeleporterToEndOfBeamline(ElementList* beamline_list)
{

#ifdef DEBUG
  G4cout << __FUNCTION_NAME__ << ": adding teleporter element to end of beamline" << G4endl;
#endif
  //based on void add_sampler in parser.h
  //create basic element with type teleporter and put on end
  struct Element e;
  e.type = _TELEPORTER;
  e.name = "endteleporter";
  e.lst  = NULL; 
  
beamline_list->push_back(e);
}

void CalculateAndSetTeleporterOffset()
{
  G4ThreeVector newteleporteroffset = G4ThreeVector(0.000163*CLHEP::m,0.0,0.000119*CLHEP::m);
  BDSGlobalConstants::Instance()->SetTeleporterOffset(newteleporteroffset);
}

void CalculateAndSetTeleporterLength()
{
  G4double teleporterlength = 0.112427*CLHEP::mm;
  BDSGlobalConstants::Instance()->SetTeleporterLength(teleporterlength);
  G4cout << "setting teleporter length to " << teleporterlength/CLHEP::mm << G4endl;
}
