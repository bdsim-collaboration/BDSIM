/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 12.12.2004
   Copyright (c) 2004 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 
#include "BDSMuSpoiler.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4TransportationManager.hh"
#include "G4UserLimits.hh"
#include "G4Tubs.hh"


#include <map>

//============================================================
typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;
//============================================================

BDSMuSpoiler::BDSMuSpoiler (G4String& aName,G4double aLength,G4double bpRad,
			    G4double rInner, G4double rOuter,G4double aBField, 
                            std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                            G4String aTunnelMaterial):
  BDSAcceleratorComponent(aName,
			 aLength,bpRad,bpRad,bpRad,
                          SetVisAttributes(), blmLocZ, blmLocTheta, aTunnelMaterial),
  itsPhysiComp(NULL),itsPhysiCompSoil(NULL),itsPhysiComp2(NULL),itsPhysiInnerBP(NULL),
  itsPhysiBP(NULL),itsSolidLogVol(NULL),itsInnerLogVol(NULL),itsBeampipeLogicalVolume(NULL),
  itsInnerBPLogicalVolume(NULL),itsBPTube(NULL),itsInnerBPTube(NULL),itsSoilTube(NULL),
  itsTunnelTube(NULL),itsInnerTunnelTube(NULL),itsInnerTunnelLogicalVolume(NULL),
  itsSoilTunnelLogicalVolume(NULL),itsTunnelUserLimits(NULL),itsSoilTunnelUserLimits(NULL),
  itsInnerTunnelUserLimits(NULL),itsBPVisAttributes(NULL),
  itsEqRhs(NULL),
  itsBeampipeRadius(bpRad),
  itsInnerRadius(rInner),
  itsOuterRadius(rOuter),
  itsBField(aBField),
  itsMagField(NULL),itsFieldMgr(NULL)
{
  G4double minInnRad =  itsBeampipeRadius + BDSGlobalConstants::Instance()->GetBeampipeThickness() + BDSGlobalConstants::Instance()->GetLengthSafety()/2.0;
  if( itsInnerRadius < minInnRad ){
    G4cout << "BDSMuSpoiler - WARNING - inner radius less than minimum. Changing inner radius from " << itsInnerRadius << " to " << minInnRad << G4endl;
    itsInnerRadius = minInnRad;
  }

  SetBPVisAttributes();
  if ( (*LogVolCount)[itsName]==0)
    {
  G4double xLength, yLength;
  G4double totalTunnelRadius = BDSGlobalConstants::Instance()->GetTunnelRadius()+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+std::max(BDSGlobalConstants::Instance()->GetTunnelOffsetX(),BDSGlobalConstants::Instance()->GetTunnelOffsetY());
  
  xLength = yLength = std::max(itsOuterRadius,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
  xLength = yLength = std::max(xLength,totalTunnelRadius);

#ifdef BDSDEBUG 
  G4cout<<"marker volume : x/y="<<xLength/CLHEP::m<<
    " m, l= "<<  (itsLength+BDSGlobalConstants::Instance()->GetLengthSafety())/2/CLHEP::m <<" m"<<G4endl;
#endif

  itsMarkerLogicalVolume=new G4LogicalVolume
    (new G4Box( itsName+"_solid",
                xLength,
		yLength,
		(itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())/2), //z half length 
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     itsName+"_log");

  // now protect the fields inside the marker volume by giving the
  // marker a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
      itsMarkerLogicalVolume->
	SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
      
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
        BuildTunnel();
      }
      BuildMuSpoiler();

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }  
}

void BDSMuSpoiler::BuildMuSpoiler()
{
  itsSolidLogVol=
    new G4LogicalVolume(new G4Tubs(itsName+"_solid",
				   itsInnerRadius+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
				   itsOuterRadius-BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
				   itsLength/2-BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
				   0,CLHEP::twopi*CLHEP::radian),
			BDSMaterials::Instance()->GetMaterial("Iron"), 
			itsName+"_solid");

  itsInnerLogVol=
    new G4LogicalVolume(new G4Tubs(itsName+"_inner",
				   0.,
				   itsInnerRadius,
				   itsLength/2,
				   0,CLHEP::twopi*CLHEP::radian),
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_inner");

  G4Material *bpMaterial = BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName() );

  // build beampipe

#ifdef BDSDEBUG 
  G4cout << "Outer pipe :"
         << " r= " << itsBpRadius/CLHEP::m << " m"
         << " l= " << itsLength/(2.)/CLHEP::m << " m"
         << G4endl;
#endif

  itsBPTube=new G4Tubs(itsName+"_bmp_solid",
		       itsBpRadius+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
		       itsBpRadius+BDSGlobalConstants::Instance()->GetBeampipeThickness(),
		       itsLength/(2.),
		       0,CLHEP::twopi*CLHEP::radian);

#ifdef BDSDEBUG
  G4cout << "Inner pipe :"
         << " r= " << (itsBpRadius-BDSGlobalConstants::Instance()->GetBeampipeThickness() )/CLHEP::m
         << " m"
         << " l= " << itsLength/(2.)/CLHEP::m << " m"
         << G4endl;
#endif
  
  itsInnerBPTube=new G4Tubs(itsName+"_inner_bmp_solid",
				0.,
				itsBpRadius,
			    itsLength/2 - BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
				0,CLHEP::twopi*CLHEP::radian);

  itsBeampipeLogicalVolume=	
    new G4LogicalVolume(itsBPTube,
			bpMaterial,
			itsName+"_bmp_log");

  itsInnerBPLogicalVolume=	
    new G4LogicalVolume(itsInnerBPTube,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_inner_bmp_log");


#ifndef NOUSERLIMITS
  G4UserLimits* AbsUserLimits =
    new G4UserLimits(DBL_MAX,DBL_MAX,BDSGlobalConstants::Instance()->GetMaxTime(),
		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());

  AbsUserLimits->SetMaxAllowedStep(itsLength);
  itsInnerLogVol->SetUserLimits(AbsUserLimits);

  itsSolidLogVol->
    SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,BDSGlobalConstants::Instance()->GetMaxTime(),
				       BDSGlobalConstants::Instance()->GetThresholdCutCharged()));

  itsInnerBPLogicalVolume->SetUserLimits(AbsUserLimits);

  itsBeampipeLogicalVolume->
    SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,BDSGlobalConstants::Instance()->GetMaxTime(),
				       BDSGlobalConstants::Instance()->GetThresholdCutCharged()));
#endif
  itsSolidLogVol->SetVisAttributes(itsVisAttributes);
  itsBeampipeLogicalVolume->SetVisAttributes(itsBPVisAttributes);

  if(itsBField)
    {
      itsMagField=new BDSMuSpoilerMagField(itsBField);
      itsFieldMgr=new G4FieldManager(itsMagField);
      itsSolidLogVol->SetFieldManager(itsFieldMgr,false);
    }
  
  itsPhysiComp2 = 
    new G4PVPlacement(
		      (G4RotationMatrix*)0,		   // no rotation
		      (G4ThreeVector)0,                   // its position
		      itsInnerLogVol,      // its logical volume
		      itsName+"_combined", // its name
		      itsSolidLogVol,      // its mother  volume
		      false,		   // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		   // copy number 

  //For geometric biasing etc.
  SetMultiplePhysicalVolumes(itsPhysiComp2);

  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    SetMultipleSensitiveVolumes(itsSolidLogVol);
  }
  if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
    SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
  }

  BuildBLMs();

  itsPhysiInnerBP = new G4PVPlacement(
				   (G4RotationMatrix*)0,		        // no rotation
				   (G4ThreeVector)0,	                // at (0,0,0)
		      itsInnerBPLogicalVolume,  // its logical volume
		      itsName+"_inner_bmp_phys",// its name
		      itsMarkerLogicalVolume,   // its mother  volume
		      false,		        // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		        // copy number
  
      itsPhysiBP = new G4PVPlacement(
				  (G4RotationMatrix*)0,			     // no rotation
				  (G4ThreeVector)0,	                     // at (0,0,0)
			  itsBeampipeLogicalVolume,  // its logical volume
			  itsName+"_bmp_phys",	     // its name
			  itsMarkerLogicalVolume,    // its mother  volume
			  false,		     // no boolean operation
				  0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number

  itsPhysiComp = 
    new G4PVPlacement(
		      (G4RotationMatrix*)0,		     // no rotation
		      (G4ThreeVector)0,                     // its position
		      itsSolidLogVol,    // its logical volume
		      itsName+"_solid",	     // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,		     // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		     // copy number  

  //For geometric biasing etc.
  SetMultiplePhysicalVolumes(itsPhysiInnerBP);
  SetMultiplePhysicalVolumes(itsPhysiBP);
  SetMultiplePhysicalVolumes(itsPhysiComp);


}

void BDSMuSpoiler::BuildBLMs(){
  itsBlmLocationR=itsOuterRadius;
  BDSAcceleratorComponent::BuildBLMs();
}


G4VisAttributes* BDSMuSpoiler::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.0,0.5,0.5));
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
  return itsVisAttributes;
}

G4VisAttributes* BDSMuSpoiler::SetBPVisAttributes()
{
  itsBPVisAttributes=new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  itsBPVisAttributes->SetForceSolid(true);
  itsBPVisAttributes->SetVisibility(true);
  return itsBPVisAttributes;
}


BDSMuSpoiler::~BDSMuSpoiler()
{
  delete itsBPVisAttributes;

//   delete itsInnerTunnelUserLimits;
//   delete itsTunnelUserLimits;
//   delete itsSoilTunnelUserLimits;

//   delete itsBPTube;
//   delete itsInnerBPTube;

  //  delete itsPhysiComp;
  //  delete itsPhysiComp2;
  //  delete itsPhysiInnerBP;
  //  delete itsPhysiBP;
  //  delete itsPhysiCompSoil;

  delete itsMagField;
  delete itsFieldMgr;

//   delete itsSoilTube;
//   delete itsTunnelTube;
//   delete itsInnerTunnelTube;
//   delete itsInnerTunnelLogicalVolume;
//   delete itsSoilTunnelLogicalVolume;
}
