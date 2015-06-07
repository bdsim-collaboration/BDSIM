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

//============================================================

BDSSpoiler::BDSSpoiler (G4String& aName,G4double aLength,G4double bpRad,
			  G4double xAper,G4double yAper,
			  G4Material* SpoilerMaterial):
  BDSAcceleratorComponent(aName,
			 aLength,bpRad,xAper,yAper),
  itsPhysiComp(NULL), itsPhysiComp2(NULL), itsSolidLogVol(NULL), 
  itsInnerLogVol(NULL), itsSpoilerMaterial(SpoilerMaterial)
{
}

void BDSSpoiler::Build()
{
  BDSAcceleratorComponent::Build();
  BuildInnerSpoiler();
}

void BDSSpoiler::BuildMarkerLogicalVolume()
{
  itsMarkerSolidVolume = new G4Box(itsName,
				   BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				   BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				   itsLength/2);
  itsMarkerLogicalVolume = new G4LogicalVolume(itsMarkerSolidVolume,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial()),
			itsName);
}


void BDSSpoiler::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
}


void BDSSpoiler::BuildInnerSpoiler()
{
  itsSolidLogVol=
    new G4LogicalVolume(new G4Box(itsName+"_solid",
				  BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				  BDSGlobalConstants::Instance()->GetComponentBoxSize()/2,
				  itsLength/2),
			itsSpoilerMaterial,
			itsName+"_solid");

  itsInnerLogVol=
    new G4LogicalVolume(new G4Box(itsName+"_inner",
				  itsXAper,
				  itsYAper,
				  itsLength/2),
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_inner");
  
  itsPhysiComp2 = 
    new G4PVPlacement(
		      (G4RotationMatrix*)0,		   // no rotation
		      (G4ThreeVector)0,                   // its position
		      itsInnerLogVol,      // its logical volume
		      itsName+"_combined", // its name
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
		      itsName+"_solid",	     // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,		     // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		     // copy number  
}


BDSSpoiler::~BDSSpoiler()
{
}
