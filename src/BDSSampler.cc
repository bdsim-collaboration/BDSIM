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
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
//#include "BDSOutput.hh"
#include "BDSSamplerSD.hh"
#include "G4SDManager.hh"

//#include"MagFieldFunction.hh"
#include <map>

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

extern BDSSamplerSD* BDSSamplerSensDet;

//extern BDSOutput* bdsOutput;
//============================================================

int BDSSampler::nSamplers = 0;

int BDSSampler::GetNSamplers() { return nSamplers; }

void BDSSampler::AddExternalSampler() { nSamplers++; }

BDSSampler::BDSSampler (G4String aName,G4double aLength):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0,
			 SetVisAttributes())
{
  nThisSampler= nSamplers + 1;
  SetName("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+itsName);
  SetType("sampler");
  SamplerLogicalVolume();
  nSamplers++;
  //G4int nSamplers=(*LogVolCount)[itsName];
  //BDSRoot->SetSamplerNumber(nSamplers);

 
}


void BDSSampler::SamplerLogicalVolume()
{
  if(!(*LogVolCount)[itsName])
    {


      itsMarkerLogicalVolume=
	new G4LogicalVolume(
			    new G4Box(itsName+"_solid",
				      BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				      BDSGlobalConstants::Instance()->GetSamplerDiameter()/2,
				      itsLength/2.0),
			    BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			    itsName);

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
#ifndef NOUSERLIMITS
      itsOuterUserLimits =new G4UserLimits();
      double stepFactor=0.5;
      itsOuterUserLimits->SetMaxAllowedStep(itsLength*stepFactor);
      itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
     // Sensitive Detector:
//SPM G4cout << "Sampler.cc Nsamplers " << bdsOutput->nSamplers << G4endl;
#ifdef DEBUG
      G4cout << "BDSSampler.cc Nsamplers " << nSamplers << G4endl;
#endif

//SPM if(bdsOutput->nSamplers==0)
      if(nSamplers==0)
	{
	  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
	  BDSSamplerSensDet=new BDSSamplerSD(itsName,"plane");
	  SDMan->AddNewDetector(BDSSamplerSensDet);
//SPM     itsMarkerLogicalVolume->SetSensitiveDetector(BDSSamplerSensDet);
	}
      itsMarkerLogicalVolume->SetSensitiveDetector(BDSSamplerSensDet);
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
      itsMarkerLogicalVolume->SetSensitiveDetector(BDSSamplerSensDet);
    }
}

G4VisAttributes* BDSSampler::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,1,1));
  return itsVisAttributes;
}

BDSSampler::~BDSSampler()
{
  --nSamplers;
}
