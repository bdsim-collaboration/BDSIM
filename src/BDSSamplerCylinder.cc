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
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "BDSSamplerSD.hh"
#include "G4SDManager.hh"

std::vector <G4String> BDSSamplerCylinder::outputNames;

int BDSSamplerCylinder::nSamplers = 0;

BDSSamplerSD* BDSSamplerCylinder::SensitiveDetector=new BDSSamplerSD("BDSSamplerCylinder","cylinder");

int BDSSamplerCylinder::GetNSamplers() { return nSamplers; }

void BDSSamplerCylinder::AddExternalSampler(G4String name) { nSamplers++; outputNames.push_back(name); }

BDSSamplerCylinder::BDSSamplerCylinder(G4String name,
				       G4double length,
				       G4double aRadius):
  BDSAcceleratorComponent(name,
			  length,
			  0,
			  "samplercylinder"),
  itsRadius(aRadius)
{
  nThisSampler = nSamplers + 1;
  nSamplers++;

  // register sampler sensitive detector
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  SDMan->AddNewDetector(SensitiveDetector);

  // to be fixed - see bdssampler for explanation
  BDSSamplerCylinder::outputNames.push_back(name + "_phys_" );
}

void BDSSamplerCylinder::BuildContainerLogicalVolume()
{
  containerSolid = new G4Tubs(name+"_body",
			      itsRadius-1.e-6*CLHEP::m,
			      itsRadius,
			      chordLength*0.5,
			      0,
			      CLHEP::twopi*CLHEP::radian);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name);
      
#ifndef NOUSERLIMITS
  G4UserLimits* itsOuterUserLimits =new G4UserLimits();
  itsOuterUserLimits->SetMaxAllowedStep(BDSGlobalConstants::Instance()->GetSamplerDiameter()/2.0);
  containerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
  containerLogicalVolume->SetSensitiveDetector(SensitiveDetector);
}

BDSSamplerCylinder::~BDSSamplerCylinder()
{
  --nSamplers;
  if(nSamplers<0) G4cerr << __METHOD_NAME__ << "WARNING: more samplers deleted than created!" << G4endl;
}
