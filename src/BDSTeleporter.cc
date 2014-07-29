#include "globals.hh" //G4 global constants & types
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"
#include "BDSTeleporter.hh"
#include "BDSMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSTeleporterStepper.hh"
#include "G4ThreeVector.hh"
#include "BDSBeamline.hh"
#include <cmath>
#include "parser/enums.h"

BDSTeleporter::BDSTeleporter(G4String name,
			     G4double length,
			     G4double apertureX,
			     G4double apertureY,
			     G4double phiAngleIn,
			     G4double phiAngleOut):
  BDSDrift(name,
	   length,
	   std::list<G4double>(), //blmLocZ,
	   std::list<G4double>(), //blmLocTheta,
	   apertureX,
	   apertureY,
	   "",     //Tunnel Material
	   true,  //Aperture set
	   0.0,    //Similar to drift in componentfactory
	   0.0,    //Tunnel Offset X
	   phiAngleIn,
	   phiAngleOut)
{
  // BuildBpFieldAndStepper();
}

void BDSTeleporter::BuildBpFieldAndStepper()
{
  G4cout << "BDSTeleporter Build Stepper & Field " << G4endl;
  // set up the magnetic field and stepper
  itsMagField = new BDSMagField(); //Zero magnetic field.
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  itsStepper  = new BDSTeleporterStepper(itsEqRhs);
}

void CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline)
{
  // get position of last item in beamline
  // and then calculate necessary offset teleporter should apply
  G4ThreeVector* lastitemposition   = thebeamline->GetLastPosition();
  G4ThreeVector* firstitemposition  = thebeamline->GetFirstPosition();
  G4ThreeVector  delta              = *lastitemposition/CLHEP::m - *firstitemposition/CLHEP::m;
#ifdef DEBUG
  G4cout << "Calculating Teleporter delta" << G4endl;
  G4cout << "last item position  : " << *lastitemposition/CLHEP::m << G4endl;
  G4cout << "first item position : " << *firstitemposition/CLHEP::m << G4endl;
  G4cout << "delta               : " << delta << G4endl;
#endif
  BDSGlobalConstants::Instance()->SetTeleporterDelta(delta*CLHEP::m);
  
  // calculate length of teleporter
  // beamline is built along z and sbend deflects in x
  // setting length here ensures that length is always z difference
  G4double lengthsafety           = BDSGlobalConstants::Instance()->GetLengthSafety();
  G4double teleporterlength       = fabs(delta.z()*CLHEP::m) - lengthsafety*CLHEP::m;
#ifdef DEBUG
  G4cout << "Calculating teleporter length" << G4endl;
  G4cout << "Length : " << teleporterlength << G4endl;
#endif  
  BDSGlobalConstants::Instance()->SetTeleporterLength(teleporterlength);
}




void AddTeleporterToEndOfBeamline(ElementList* beamline_list)
{
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << ": adding teleporter element to end of beamline" << G4endl;
#endif
  //based on void add_sampler in parser.h
  //create basic element with type teleporter and put on end
  struct Element e;
  e.type = _TELEPORTER;
  e.name = "Teleporter";
  e.lst  = NULL; 
  beamline_list->push_back(e);
}

