/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 
#include "BDSSpoiler.hh"
#include "BDSMaterials.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"

class BDSTiltOffset;

BDSSpoiler::BDSSpoiler(G4String      name,
		       G4double      length,
		       G4double      xAperIn,
		       G4double      yAperIn,
		       G4Material*   SpoilerMaterial,
		       BDSTiltOffset tiltOffset):
  BDSAcceleratorComponent(name, length, 0, "spoiler", tiltOffset),
  itsPhysiComp(NULL), itsPhysiComp2(NULL), itsSolidLogVol(NULL), 
  itsInnerLogVol(NULL), itsSpoilerMaterial(SpoilerMaterial),
  xAper(xAperIn),
  yAper(yAperIn)
{
}

void BDSSpoiler::Build()
{
  BDSAcceleratorComponent::Build();
  BuildInnerSpoiler();
}

void BDSSpoiler::BuildMarkerLogicalVolume()
{
  containerSolid = new G4Box(name,
			     BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
			     BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
			     chordLength/2);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSSpoiler::BuildInnerSpoiler()
{
  itsSolidLogVol=
    new G4LogicalVolume(new G4Box(name+"_solid",
				  BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				  BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				  chordLength/2),
			itsSpoilerMaterial,
			name+"_solid");

  itsInnerLogVol=
    new G4LogicalVolume(new G4Box(name+"_inner",
				  xAper,
				  yAper,
				  chordLength/2),
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			name+"_inner");
  
  itsPhysiComp2 = 
    new G4PVPlacement(
		      (G4RotationMatrix*)0,		   // no rotation
		      (G4ThreeVector)0,                   // its position
		      itsInnerLogVol,      // its logical volume
		      name+"_combined", // its name
		      itsSolidLogVol,      // its mother  volume
		      false,		   // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());  // copy number 

  if(BDSGlobalConstants::Instance()->GetSensitiveComponents())
    {RegisterSensitiveVolume(itsSolidLogVol);}

#ifndef NOUSERLIMITS
  itsSolidLogVol->
    SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,BDSGlobalConstants::Instance()->GetMaxTime(),
				   BDSGlobalConstants::Instance()-> GetThresholdCutCharged()));
#endif
  itsPhysiComp = 
    new G4PVPlacement(
		      (G4RotationMatrix*)0,		     // no rotation
		      (G4ThreeVector)0,                     // its position
		      itsSolidLogVol,    // its logical volume
		      name+"_solid",	     // its name
		      containerLogicalVolume, // its mother  volume
		      false,		     // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		     // copy number  
}


BDSSpoiler::~BDSSpoiler()
{
}
