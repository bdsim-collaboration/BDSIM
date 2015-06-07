//  
//   BDSIM, (C) 2001-2007
//    
//   version 0.3 
//   last modified : 08 May 2007 by agapov@pp.rhul.ac.uk
//  


//
//    beam dumper/reader for online exchange with external codes
//

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDump.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "BDSDumpSD.hh"
#include "BDSMaterials.hh"
#include "G4SDManager.hh"

BDSDumpSD* BDSDumpSensDet;

int BDSDump::nDumps=0;

BDSDump::BDSDump (G4String name, G4double length):
  BDSAcceleratorComponent(name, length, 0, "dump")
{
  //SetName("Dump_"+BDSGlobalConstants::Instance()->StringFromInt(nDumps)+"_"+itsName);
  ++nDumps;
  //BDSRoot->SetDumpNumber(nDumps);
}

void BDSDump::BuildContainerLogicalVolume()
{
  G4double radius = BDSGlobalConstants::Instance()->GetSamplerDiameter() * 0.5;
  
  containerSolid = new G4Box(name + "_container_solid",
			     radius,
			     radius,
			     chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");

#ifndef NOUSERLIMITS
  G4UserLimits* itsOuterUserLimits = new G4UserLimits();
  itsOuterUserLimits->SetMaxAllowedStep(chordLength);
  itsOuterUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  containerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
  // Sensitive Detector:
  if(nDumps==0)
    {
      G4SDManager* SDMan = G4SDManager::GetSDMpointer();
      BDSDumpSensDet=new BDSDumpSD(name,"plane");
      SDMan->AddNewDetector(BDSDumpSensDet);
    }
  containerLogicalVolume->SetSensitiveDetector(BDSDumpSensDet);
}

BDSDump::~BDSDump()
{
  nDumps--;
}
