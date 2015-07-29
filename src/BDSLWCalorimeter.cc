#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSLWCalorimeter.hh"
#include "BDSSDManager.hh"
#include "BDSMaterials.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"


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
  G4UserLimits* outerUserLimits =new G4UserLimits();
  outerUserLimits->SetMaxAllowedStep(chordLength);
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  RegisterUserLimits(outerUserLimits);
  containerLogicalVolume->SetUserLimits(outerUserLimits);
#endif
}

void BDSLWCalorimeter::BuildCal(G4double aLength)
{
  // build the Calorimeter
  itsLWCal=new G4Box(name + "_lw_cal_solid",
		     BDSGlobalConstants::Instance()->GetLWCalWidth()/2,
		     BDSGlobalConstants::Instance()->GetLWCalWidth()/2,
		     aLength/2);
  RegisterSolid(itsLWCal);
  itsLWCalLogicalVolume=new G4LogicalVolume(itsLWCal,
					    BDSMaterials::Instance()->GetMaterial("LeadTungstate"),
					    name + "_lw_cal_lv");
  RegisterLogicalVolume(itsLWCalLogicalVolume);
  itsPhysiLWCal = new G4PVPlacement(0,                       // rotation
				    G4ThreeVector(BDSGlobalConstants::Instance()->GetLWCalOffset(),0.,0.),
				    itsLWCalLogicalVolume,   // its logical volume
				    name +"_lw_cal_pv",	     // its name
				    containerLogicalVolume,  // its mother  volume
				    false,		     // no boolean operation
				    0,                       // copy number
				    checkOverlaps);
  RegisterPhysicalVolume(itsPhysiLWCal);
  
  itsLWCalLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetLWCalorimeterSD());    
}

void BDSLWCalorimeter::BuildBeampipe()
{
  BDSBeamPipe* pipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
								     chordLength,
								     beamPipeInfo);

  // register logical volumes using geometry component base class
  InheritObjects(pipe);  

  G4PVPlacement* beampipePV = new G4PVPlacement(0,                                 // rotation
						(G4ThreeVector)0,                  // position
						pipe->GetContainerLogicalVolume(), // its logical volume
						name +"_beampipe_pv",              // its name
						containerLogicalVolume,            // its mother  volume
						false,		                   // no boolean operation
						0,
						checkOverlaps);                    // copy number

  RegisterPhysicalVolume(beampipePV);
  
  // Set extents
  SetExtentX(pipe->GetExtentX());
  SetExtentY(pipe->GetExtentY());
  SetExtentZ(pipe->GetExtentZ());
}

BDSLWCalorimeter::~BDSLWCalorimeter()
{;}
