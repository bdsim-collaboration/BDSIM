/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 

#include "BDSDrift.hh"

#include "BDSDriftStepper.hh"
#include "BDSMagField.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <map>

//============================================================

BDSDrift::BDSDrift (G4String aName, 
		    G4double aLength, 
                    std::list<G4double> blmLocZ, 
		    std::list<G4double> blmLocTheta, 
		    G4double aperX, 
		    G4double aperY, 
		    G4String tunnelMaterial, 
		    G4bool   aperset, 
		    G4double aper, 
		    G4double tunnelOffsetX, 
		    G4double phiAngleIn, 
		    G4double phiAngleOut):
  BDSMultipole(aName, 
	       aLength, 
	       aper, 
	       aper, 
	       blmLocZ, 
	       blmLocTheta, 
	       tunnelMaterial, 
	       "", 
	       aperX, 
	       aperY, 
	       0, 
	       0, 
	       tunnelOffsetX, 
	       phiAngleIn, 
	       phiAngleOut),
  itsStartOuterR(0.0),
  itsEndOuterR(0.0),
  itsAperset(aperset)
{
  if(!itsAperset){
    itsStartOuterR=aperX + BDSGlobalConstants::Instance()->GetBeampipeThickness();
    itsEndOuterR=aperY + BDSGlobalConstants::Instance()->GetBeampipeThickness();
    SetStartOuterRadius(itsStartOuterR);
    SetEndOuterRadius(itsEndOuterR);
  }
}

void BDSDrift::BuildBeampipe(G4String materialName) {
  if (itsAperset){
    BDSMultipole::BuildBeampipe(materialName);
  } else {
    BDSMultipole::BuildBeampipe(itsXAper, itsYAper, materialName);
  }
}

void BDSDrift::Build() {
  BDSMultipole::Build();

  static G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  VisAtt1->SetVisibility(false);
  VisAtt1->SetForceSolid(true);
  itsMarkerLogicalVolume->SetVisAttributes(VisAtt1);
}

void BDSDrift::BuildBPFieldAndStepper(){
    // set up the magnetic field and stepper
  itsMagField = new BDSMagField(); //Zero magnetic field.
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  itsStepper  = new BDSDriftStepper(itsEqRhs);
}

void BDSDrift::BuildBLMs(){
  itsBlmLocationR = std::max(itsStartOuterR, itsEndOuterR) - itsBpRadius;
  BDSAcceleratorComponent::BuildBLMs(); // resets itsBlmLocationR! -- JS
}

BDSDrift::~BDSDrift()
{
}
