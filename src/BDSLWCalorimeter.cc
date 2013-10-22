/* BDSIM code for LW Calorimeter.    Version 1.0
   Author: John Carter, Royal Holloway, Univ. of London.
   Last modified 26.7.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh" 
#include "BDSLWCalorimeter.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"

#include "BDSAcceleratorComponent.hh"

#include "BDSLWCalorimeterSD.hh"
#include "G4SDManager.hh"


//#include"MagFieldFunction.hh"
#include <map>


//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;


//============================================================

BDSLWCalorimeter::BDSLWCalorimeter (G4String& aName,G4double aLength,
				    G4double aBpRad, G4String aTunnelMaterial):
  BDSAcceleratorComponent(
			 aName,
			 aLength,aBpRad,0,0,
			 SetVisAttributes(),aTunnelMaterial),
  itsBPFieldMgr(NULL),
  itsVisAttributes(NULL)
{
  LWCalorimeterLogicalVolume();
  BuildCal(aLength);
  BuildBeampipe(aLength);

  //G4int nLWCalorimeters=(*LogVolCount)[itsName];
  //BDSRoot->SetLWCalorimeterNumber(nLWCalorimeters);

}


void BDSLWCalorimeter::LWCalorimeterLogicalVolume()
{
  if(!(*LogVolCount)[itsName])
    {

      G4double SampTransSize;
      SampTransSize=2.*BDSGlobalConstants::Instance()->GetTunnelRadius();

      itsMarkerLogicalVolume=
	new G4LogicalVolume(
			    new G4Box(itsName+"_solid",
				      SampTransSize,
				      SampTransSize,
				      itsLength/2),
			    BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			    itsName);

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
#ifndef NOUSERLIMITS
      itsOuterUserLimits =new G4UserLimits();
      itsOuterUserLimits->SetMaxAllowedStep(itsLength);
      itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
      itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif

    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
}

void BDSLWCalorimeter::BuildCal(G4double aLength)
{
  // build the Calorimeter
   
  itsLWCal=new G4Box(itsName+"_LWCal",
		     BDSGlobalConstants::Instance()->GetLWCalWidth()/2,
		     BDSGlobalConstants::Instance()->GetLWCalWidth()/2,
		     aLength/2);
  itsLWCalLogicalVolume=new G4LogicalVolume(itsLWCal,
					    BDSMaterials::Instance()->GetMaterial("LeadTungstate"),
					    itsName+"_cal_logical");
  G4RotationMatrix* Rot=NULL;
  if(itsAngle!=0)Rot=BDSGlobalConstants::Instance()->RotY90();
 
  itsPhysiLWCal = new G4PVPlacement(
		    Rot,                     // rotation
		    G4ThreeVector(BDSGlobalConstants::Instance()->GetLWCalOffset(),0.,0.),
		    itsLWCalLogicalVolume,   // its logical volume
		    itsName+"_cal",	     // its name
		    itsMarkerLogicalVolume,  // its mother  volume
		    false,		     // no boolean operation
		    0, BDSGlobalConstants::Instance()->GetCheckOverlaps()); // copy number
  
  // Sensitive Detector:
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
 
  BDSLWCalorimeterSD* SensDet=new BDSLWCalorimeterSD(itsName);
  SDMan->AddNewDetector(SensDet);
  
  itsLWCalLogicalVolume->SetSensitiveDetector(SensDet);    
}
void BDSLWCalorimeter::BuildBeampipe(G4double aLength)
{
  // build beampipe
  itsBPTube=new G4Tubs(itsName+"_tube",
		       0.,itsBpRadius,
		       aLength/2,
		       0,twopi*radian);
  
  itsInnerBPTube=new G4Tubs(itsName+"_InnerTube",
			    0.,
			    itsBpRadius-BDSGlobalConstants::Instance()->GetBeampipeThickness(),
			    aLength/2,
			    0,twopi*radian);
  itsBeampipeLogicalVolume=	
    new G4LogicalVolume(itsBPTube,
			//			BDSMaterials::Instance()->("Iron"),
			BDSMaterials::Instance()->GetMaterial("Aluminium"),
			itsName+"_bmp_logical");
  
  itsInnerBPLogicalVolume=	
    new G4LogicalVolume(itsInnerBPTube,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_bmp_Inner_log");
  
  itsPhysiInner = new G4PVPlacement(
		      (G4RotationMatrix*)0,     // rotation
		      (G4ThreeVector)0,         // at (0,0,0)
		      itsInnerBPLogicalVolume,  // its logical volume
		      itsName+"_InnerBmp",      // its name
		      itsBeampipeLogicalVolume, // its mother  volume
		      false,		       // no boolean operation
				  0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		       // copy number
  
  
   G4RotationMatrix* Rot=NULL;
   if(itsAngle!=0)Rot=BDSGlobalConstants::Instance()->RotY90();
  
   itsPhysiComp = new G4PVPlacement(
		       Rot,                       // rotation
		       (G4ThreeVector)0,          // at (0,0,0)
		       itsBeampipeLogicalVolume,  // its logical volume
		       itsName+"_bmp",            // its name
		       itsMarkerLogicalVolume,    // its mother  volume
		       false,	                  // no boolean operation
		       0, BDSGlobalConstants::Instance()->GetCheckOverlaps()); // copy number
#ifndef NOUSERLIMITS
   itsBeampipeUserLimits =
     new G4UserLimits("beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
		      BDSGlobalConstants::Instance()->GetThresholdCutCharged());
   
   itsInnerBeampipeUserLimits =
     new G4UserLimits("inner beamipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
		      BDSGlobalConstants::Instance()->GetThresholdCutCharged());
   
   itsBeampipeUserLimits->SetMaxAllowedStep(itsLength);
   itsBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
   
   itsBeampipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);
   
   itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength);
   itsInnerBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
   
   itsInnerBPLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
#endif
   itsInnerBPLogicalVolume->SetFieldManager(itsBPFieldMgr,false) ;
   
   // now protect the fields inside the marker volume by giving the
   // marker a null magnetic field (otherwise G4VPlacement can
   // over-ride the already-created fields, by calling 
   // G4LogicalVolume::AddDaughter, which calls 
   // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
   // latter 'true' over-writes all the other fields
   
   itsMarkerLogicalVolume->
     SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  
}

G4VisAttributes* BDSLWCalorimeter::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,0.5,0.5));
  return itsVisAttributes;
}

BDSLWCalorimeter::~BDSLWCalorimeter()
{
  delete itsVisAttributes;
}
