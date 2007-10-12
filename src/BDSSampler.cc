/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed Samplers to account for plane and cylinder types (GABs code)
*/
// gab:
#include "BDSGlobalConstants.hh" // must be first in include list
#include "BDSSampler.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "BDSOutput.hh"
#include "BDSSamplerSD.hh"
#include "G4SDManager.hh"

//#include"MagFieldFunction.hh"
#include <map>



typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

extern BDSSamplerSD* BDSSamplerSensDet;

extern BDSOutput bdsOutput;
extern BDSMaterials* theMaterials;
//============================================================

int BDSSampler::nSamplers = 0;

const int BDSSampler::GetNSamplers() { return nSamplers; }

void BDSSampler::AddExternalSampler() { nSamplers++; }

BDSSampler::BDSSampler (G4String aName,G4double aLength):
  BDSAcceleratorComponent(
			 aName,
			 aLength,0,0,0,
			 SetVisAttributes())
{
  nThisSampler= nSamplers + 1;
  SetName("Sampler_"+BDSGlobals->StringFromInt(nThisSampler)+"_"+itsName);
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

      G4double SampTransSize;
      SampTransSize=2.*BDSGlobals->GetTunnelRadius();

      itsMarkerLogicalVolume=
	new G4LogicalVolume(
			    new G4Box(itsName+"_solid",
				      SampTransSize,
				      SampTransSize,
				      itsLength/2.0),
			    theMaterials->GetMaterial("Vacuum"),
			    itsName);

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;

      itsOuterUserLimits =new G4UserLimits();
      itsOuterUserLimits->SetMaxAllowedStep(itsLength);
      itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);

     // Sensitive Detector:
//SPM G4cout << "Sampler.cc Nsamplers " << bdsOutput.nSamplers << G4endl;
      G4cout << "Sampler.cc Nsamplers " << nSamplers << G4endl;

//SPM if(bdsOutput.nSamplers==0)
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
  if(itsVisAttributes) delete itsVisAttributes;
  if(itsUserLimits) delete itsUserLimits;
  --nSamplers;
}
