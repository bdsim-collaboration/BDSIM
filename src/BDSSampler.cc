/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed Samplers to account for plane and cylinder types (GABs code)
*/

#include "BDSGlobalConstants.hh" 
#include "BDSExecOptions.hh"
#include "BDSMaterials.hh"
#include "BDSSampler.hh"
#include "BDSDebug.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "BDSSamplerSD.hh"

#include "BDSSDManager.hh"

//============================================================

std::vector <G4String> BDSSampler::outputNames;

int BDSSampler::nSamplers = 0;

// created here, so only one is created with fixed known name
BDSSamplerSD* BDSSampler::SensitiveDetector = new BDSSamplerSD("BDSSampler","plane");

int BDSSampler::GetNSamplers() { return nSamplers; }

void BDSSampler::AddExternalSampler(G4String name) { nSamplers++; outputNames.push_back(name); }

BDSSampler::BDSSampler (G4String aName, G4double aLength):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  nThisSampler= nSamplers + 1;
  SetName("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+itsName);
  nSamplers++;
#ifdef BDSDEBUG
  G4cout << "BDSSampler.cc Nsamplers " << nSamplers << G4endl;
#endif
}

void BDSSampler::Initialise()
{
  BDSAcceleratorComponent::Initialise();
  
  BDSSampler::outputNames.push_back(itsName + "_phys_" + BDSGlobalConstants::Instance()->StringFromInt(GetCopyNumber()+1));
}

void BDSSampler::BuildMarkerLogicalVolume()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  itsMarkerSolidVolume = new G4Box(itsName+"_solid",
				   BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				   BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				   itsLength/2.0);
  itsMarkerLogicalVolume = new G4LogicalVolume(itsMarkerSolidVolume,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial()),
					       itsName);
  
#ifndef NOUSERLIMITS
  itsOuterUserLimits =new G4UserLimits();
  //      double stepFactor=5;
  //      itsOuterUserLimits->SetMaxAllowedStep(itsLength*stepFactor);
  itsOuterUserLimits->SetMaxAllowedStep(1*CLHEP::m);
  itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
  //itsMarkerLogicalVolume->SetSensitiveDetector(SensitiveDetector);
  if (BDSExecOptions::Instance()->GetVisDebug())
    {itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}
  
  itsMarkerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerPlaneSD());
}

BDSSampler::~BDSSampler()
{
  --nSamplers;

  if(nSamplers<0) G4cerr << __METHOD_NAME__ << "WARNING: more samplers deleted than created!" << G4endl;
}
