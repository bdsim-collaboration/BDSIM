/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 5.3.2005
   Copyright (c) 2005 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 
#include "BDSMaterials.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSDebug.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "BDSSamplerSD.hh"
#include "G4SDManager.hh"

//============================================================

std::vector <G4String> BDSSamplerCylinder::outputNames;

int BDSSamplerCylinder::nSamplers = 0;

BDSSamplerSD* BDSSamplerCylinder::SensitiveDetector=new BDSSamplerSD("BDSSamplerCylinder","cylinder");

int BDSSamplerCylinder::GetNSamplers() { return nSamplers; }

void BDSSamplerCylinder::AddExternalSampler(G4String name) { nSamplers++; outputNames.push_back(name); }

BDSSamplerCylinder::
BDSSamplerCylinder (G4String aName,G4double aLength,G4double aRadius):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0),
  itsRadius(aRadius)
{
  nThisSampler = nSamplers + 1;
  SetName("CSampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+itsName);
  nSamplers++;

  // register sampler sensitive detector
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  SDMan->AddNewDetector(SensitiveDetector);
}

void BDSSamplerCylinder::Initialise()
{
  BDSAcceleratorComponent::Initialise();
  
  BDSSamplerCylinder::outputNames.push_back(itsName + "_phys_" + BDSGlobalConstants::Instance()->StringFromInt(GetCopyNumber()+1));
}

void BDSSamplerCylinder::BuildMarkerLogicalVolume()
{
  itsMarkerLogicalVolume=
    new G4LogicalVolume(new G4Tubs(itsName+"_body",
				   itsRadius-1.e-6*CLHEP::m,
				   itsRadius,
				   itsLength/2,
				   0,CLHEP::twopi*CLHEP::radian),
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial()),
			itsName);
      
#ifndef NOUSERLIMITS
  itsOuterUserLimits =new G4UserLimits();
  itsOuterUserLimits->SetMaxAllowedStep(BDSGlobalConstants::Instance()->GetSamplerDiameter()/2.0);
  itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
  itsMarkerLogicalVolume->SetSensitiveDetector(SensitiveDetector);
}

void BDSSamplerCylinder::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,0,1));
}

BDSSamplerCylinder::~BDSSamplerCylinder()
{
  --nSamplers;
  if(nSamplers<0) G4cerr << __METHOD_NAME__ << "WARNING: more samplers deleted than created!" << G4endl;
}
