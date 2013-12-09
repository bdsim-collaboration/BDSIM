/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 5.3.2005
   Copyright (c) 2005 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 
#include "BDSSamplerCylinder.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"

#include <map>

#include "BDSSamplerSD.hh"
#include "G4SDManager.hh"


typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;



//============================================================

int BDSSamplerCylinder::nSamplers = 0;

int BDSSamplerCylinder::GetNSamplers() { return nSamplers; }

void BDSSamplerCylinder::AddExternalSampler() { nSamplers++; }

BDSSamplerCylinder::
BDSSamplerCylinder (G4String aName,G4double aLength,G4double aRadius):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0,
			 SetVisAttributes()),
  itsRadius(aRadius),itsVisAttributes(NULL)
{
  nThisSampler = nSamplers + 1;
  SetName("CSampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+itsName);
  SetType("csampler");
  SamplerCylinderLogicalVolume();
  nSamplers++;
  //G4int nSamplers=(*LogVolCount)[itsName];
  //BDSRoot->SetSampCylinderNumber(nSamplers);
}


void BDSSamplerCylinder::SamplerCylinderLogicalVolume()
{
  if(!(*LogVolCount)[itsName])
    {

      itsMarkerLogicalVolume=
	new G4LogicalVolume(new G4Tubs(itsName+"_body",
				       itsRadius-1.e-6*CLHEP::m,
				       itsRadius,
				       itsLength/2,
				       0,CLHEP::twopi*CLHEP::radian),
			    BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			    itsName);
      
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
#ifndef NOUSERLIMITS
      itsOuterUserLimits =new G4UserLimits();
      itsOuterUserLimits->SetMaxAllowedStep(BDSGlobalConstants::Instance()->GetSamplerDiameter()/2.0);
      itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
      // Sensitive Detector:
      G4SDManager* SDMan = G4SDManager::GetSDMpointer();
      BDSSamplerSD* SensDet=new BDSSamplerSD(itsName,"cylinder");
       
      SDMan->AddNewDetector(SensDet);
      itsMarkerLogicalVolume->SetSensitiveDetector(SensDet);
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
}

G4VisAttributes* BDSSamplerCylinder::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,0,1));
  return itsVisAttributes;
}

BDSSamplerCylinder::~BDSSamplerCylinder()
{
  delete itsVisAttributes;
  --nSamplers;
}
