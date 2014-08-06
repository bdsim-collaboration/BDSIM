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

//#include"MagFieldFunction.hh"
#include <map>



typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;
BDSDumpSD* BDSDumpSensDet;

//============================================================

BDSDump::BDSDump (G4String aName,G4double aLength, G4String aTunnelMaterial):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0,
			 SetVisAttributes(), aTunnelMaterial)
{
  SetName("Dump_"+BDSGlobalConstants::Instance()->StringFromInt(nDumps)+"_"+itsName);
  DumpLogicalVolume();
  ++nDumps;
  //G4int nDumps=(*LogVolCount)[itsName];
  //BDSRoot->SetDumpNumber(nDumps);
}

int BDSDump::GetNumberOfDumps()
{
  return nDumps;
}

int BDSDump::nDumps=0;

int BDSDump::nUsedDumps=0;

void BDSDump::DumpLogicalVolume()
{
  if(!(*LogVolCount)[itsName])
    {

      G4double SampTransSize;
      SampTransSize=BDSGlobalConstants::Instance()->GetSamplerDiameter()/2.0;

      itsMarkerLogicalVolume=
	new G4LogicalVolume(
			    new G4Box(itsName+"_solid",
				      SampTransSize,
				      SampTransSize,
				      itsLength/2.0),
			    BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			    itsName);

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
#ifndef NOUSERLIMITS
      itsOuterUserLimits =new G4UserLimits();
      itsOuterUserLimits->SetMaxAllowedStep(itsLength);
      itsOuterUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
      itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
      itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
      // Sensitive Detector:
      if(nDumps==0)
	{
	  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
	  BDSDumpSensDet=new BDSDumpSD(itsName,"plane");
	  SDMan->AddNewDetector(BDSDumpSensDet);
	}
      itsMarkerLogicalVolume->SetSensitiveDetector(BDSDumpSensDet);
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
      itsMarkerLogicalVolume->SetSensitiveDetector(BDSDumpSensDet);
    }
}

G4VisAttributes* BDSDump::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,1,1));
  return itsVisAttributes;
}

BDSDump::~BDSDump()
{
  nDumps--;
}
