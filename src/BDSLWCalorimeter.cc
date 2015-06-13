/* BDSIM code for LW Calorimeter.    Version 1.0
   Author: John Carter, Royal Holloway, Univ. of London.
   Last modified 26.7.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSLWCalorimeter.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"

#include "BDSAcceleratorComponent.hh"

#include "BDSLWCalorimeterSD.hh"
#include "G4SDManager.hh"

BDSLWCalorimeter::BDSLWCalorimeter(G4String         name,
				   G4double         length,
				   BDSBeamPipeInfo* beamPipeInfoIn):
  BDSAcceleratorComponent(name, length, 0, "lwcalorimeter"),
  itsBeampipeLogicalVolume(NULL),itsInnerBPLogicalVolume(NULL),itsPhysiInner(NULL),
  itsPhysiComp(NULL),itsLWCalLogicalVolume(NULL),itsBeampipeUserLimits(NULL),
  itsBPFieldMgr(NULL),itsBPTube(NULL),itsInnerBPTube(NULL),itsLWCal(NULL),
  itsPhysiLWCal(NULL)
{
  beamPipeInfo = beamPipeInfoIn;
}

void BDSLWCalorimeter::Build()
{
  BDSAcceleratorComponent::Build();
  BuildCal(chordLength);
  BuildBeampipe();
}

void BDSLWCalorimeter::BuildMarkerLogicalVolume()
{
  G4double SampTransSize;
  SampTransSize = BDSGlobalConstants::Instance()->GetSamplerDiameter() * 0.5 * CLHEP::mm;

  containerSolid = new G4Box(name + "_container_solid", // name
			     SampTransSize,
			     SampTransSize,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  
#ifndef NOUSERLIMITS
  G4UserLimits* itsOuterUserLimits =new G4UserLimits();
  itsOuterUserLimits->SetMaxAllowedStep(chordLength);
  itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  containerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

void BDSLWCalorimeter::BuildCal(G4double aLength)
{
  // build the Calorimeter
  itsLWCal=new G4Box(name + "_lw_cal_solid",
		     BDSGlobalConstants::Instance()->GetLWCalWidth()/2,
		     BDSGlobalConstants::Instance()->GetLWCalWidth()/2,
		     aLength/2);
  itsLWCalLogicalVolume=new G4LogicalVolume(itsLWCal,
					    BDSMaterials::Instance()->GetMaterial("LeadTungstate"),
					    name + "_lw_cal_lv");
  G4RotationMatrix* Rot=NULL;
  if(angle!=0)Rot=BDSGlobalConstants::Instance()->RotY90();
 
  itsPhysiLWCal = new G4PVPlacement(Rot,                     // rotation
				    G4ThreeVector(BDSGlobalConstants::Instance()->GetLWCalOffset(),0.,0.),
				    itsLWCalLogicalVolume,   // its logical volume
				    name +"_lw_cal_pv",	     // its name
				    containerLogicalVolume,  // its mother  volume
				    false,		     // no boolean operation
				    0,
				    BDSGlobalConstants::Instance()->GetCheckOverlaps()); // copy number
  
  // Sensitive Detector:
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
 
  BDSLWCalorimeterSD* SensDet=new BDSLWCalorimeterSD(name);
  SDMan->AddNewDetector(SensDet);
  
  itsLWCalLogicalVolume->SetSensitiveDetector(SensDet);    
}
void BDSLWCalorimeter::BuildBeampipe()
{
  BDSBeamPipe* pipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
								     chordLength,
								     beamPipeInfo);
  if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe())
    {RegisterSensitiveVolumes(pipe->GetAllSensitiveVolumes());}
  

  new G4PVPlacement(0,                                 // rotation
		    (G4ThreeVector)0,                  // position
		    pipe->GetContainerLogicalVolume(), // its logical volume
		    name +"_beampipe_pv",              // its name
		    containerLogicalVolume,            // its mother  volume
		    false,		               // no boolean operation
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()); // copy number

  // Set extents
  SetExtentX(pipe->GetExtentX());
  SetExtentY(pipe->GetExtentY());
  SetExtentZ(pipe->GetExtentZ());
}

BDSLWCalorimeter::~BDSLWCalorimeter()
{
}
