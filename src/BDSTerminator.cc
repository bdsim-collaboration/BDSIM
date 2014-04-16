/* BDSIM
   Author: L. Nevay,
   Last modified 15/04/2014
   Copyright (c) 2014.  ALL RIGHTS RESERVED.
*/

#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSTerminator.hh"
#include "BDSTerminatorSD.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"

#include <map>

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

BDSTerminator::BDSTerminator(G4String aName, G4double aLength):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0,
			 SetVisAttributes())
{
  nThisSampler= nSamplers + 1;
  SetName("Terminator_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+itsName);
  SetType("terminator");
  TerminatorLogicalVolume();
  nSamplers++;
}

void BDSTerminator::TerminatorLogicalVolume()
{
  //if(!(*LogVolCount)[itsName])
  //  {
      G4Box* terminatorBox = new G4Box(itsName+"_solid",
				       BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				       BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				       itsLength/2.0);
      itsMarkerLogicalVolume =new G4LogicalVolume(terminatorBox,
						  BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
						  itsName);

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;

      //OLD CODE BELOW - perhaps where to put in user limits logic
      //#ifndef NOUSERLIMITS
      //itsOuterUserLimits =new G4UserLimits();
      //double stepFactor=0.5;
      //itsOuterUserLimits->SetMaxAllowedStep(itsLength*stepFactor);
      //itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
      //#endif

      G4SDManager* SDMan    = G4SDManager::GetSDMpointer();
      G4VSensitiveDetector* theTerminator  = new BDSTerminatorSD(itsName);
      SDMan->AddNewDetector(theTerminator);
      itsMarkerLogicalVolume->SetSensitiveDetector(theTerminator);
      //  }
  /*  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
      itsMarkerLogicalVolume->SetSensitiveDetector(theTerminator);
      }*/
}

G4VisAttributes* BDSTerminator::SetVisAttributes()
{
  itsVisAttributes = new G4VisAttributes(G4Colour(1,1,1));
  return itsVisAttributes;
}

BDSTerminator::~BDSTerminator()
{
  --nSamplers;
}
