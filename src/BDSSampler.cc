/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed Samplers to account for plane and cylinder types (GABs code)
*/
// gab:
#include "BDSGlobalConstants.hh" 
#include "BDSSampler.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "BDSSamplerSD.hh"
#include "G4SDManager.hh"

//#include"MagFieldFunction.hh"
#include <map>

extern BDSSamplerSD* BDSSamplerSensDet;

std::vector <G4String> BDSSampler::outputNames;

//============================================================

int BDSSampler::nSamplers = 0;

int BDSSampler::GetNSamplers() { return nSamplers; }

void BDSSampler::AddExternalSampler(G4String name) { nSamplers++; outputNames.push_back(name); }

BDSSampler::BDSSampler (G4String aName, G4double aLength):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0,
			 SetVisAttributes())
{
  nThisSampler= nSamplers + 1;
  SetName("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+itsName);
  SetType("sampler");
  nSamplers++;
#ifdef BDSDEBUG
  G4cout << "BDSSampler.cc Nsamplers " << nSamplers << G4endl;
#endif
  //BDSRoot->SetSamplerNumber(nSamplers); 
}

void BDSSampler::BuildMarkerLogicalVolume()
{
  itsMarkerLogicalVolume=
    new G4LogicalVolume(
			new G4Box(itsName+"_solid",
				  BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				  BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				  itsLength/2.0),
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName);
  
#ifndef NOUSERLIMITS
  itsOuterUserLimits =new G4UserLimits();
  //      double stepFactor=5;
  //      itsOuterUserLimits->SetMaxAllowedStep(itsLength*stepFactor);
  itsOuterUserLimits->SetMaxAllowedStep(1*CLHEP::m);
  itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
  // Sensitive Detector:
  if(nSamplers==0)
    {
      G4SDManager* SDMan = G4SDManager::GetSDMpointer();
      BDSSamplerSensDet=new BDSSamplerSD(itsName,"plane");
      SDMan->AddNewDetector(BDSSamplerSensDet);
    }
  itsMarkerLogicalVolume->SetSensitiveDetector(BDSSamplerSensDet);
}

G4VisAttributes* BDSSampler::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.5,0.6,0.7));
#if defined BDSDEBUG
  itsVisAttributes->SetVisibility(true);
#else
  itsVisAttributes->SetVisibility(false);
#endif
  return itsVisAttributes;
}

BDSSampler::~BDSSampler()
{
  --nSamplers;
}
