/* BDSIM code.    Version beta
   Author: Grahame I. Agapov, Royal Holloway, Univ. of London.

*/
#include "BDSGlobalConstants.hh" 
#include "BDSCollimator.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4EllipticalTube.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"

#include "G4UserLimits.hh"
#include "parser/enums.h"
#include <map>

//============================================================
typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSCollimator::BDSCollimator (G4String aName,G4double aLength,G4double bpRad,
			      G4double xAper,G4double yAper, G4int type,
			      G4Material *CollimatorMaterial, G4double outR, 
                              std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                              G4String aTunnelMaterial):
  BDSAcceleratorComponent(aName,
			  aLength,bpRad,xAper,yAper,
			  SetVisAttributes(), blmLocZ, blmLocTheta, aTunnelMaterial),
  itsPhysiComp(NULL), itsPhysiComp2(NULL), itsSolidLogVol(NULL), itsTempSolidLogVol(NULL),
  itsInnerLogVol(NULL), itsInnerSolid(NULL), itsOuterSolid(NULL), itsSolid(NULL), itsSoilTube(NULL),
  itsTunnelTube(NULL),  itsInnerTunnelTube(NULL), itsInnerTunnelLogicalVolume(NULL),
  itsSoilTunnelLogicalVolume(NULL), itsTunnelUserLimits(NULL), itsSoilTunnelUserLimits(NULL),
  itsInnerTunnelUserLimits(NULL), itsEqRhs(NULL),
  itsCollimatorMaterial(CollimatorMaterial), itsOuterR(outR)
{
  if(type==_RCOL) itsType="rcol";
  if(type==_ECOL) itsType="ecol";

  if(itsOuterR==0) itsOuterR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;

  if ( (*LogVolCount)[itsName]==0)
    {
  G4double xLength, yLength;
  xLength = yLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);

  xLength = std::max(xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  yLength = std::max(yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  itsMarkerLogicalVolume=new G4LogicalVolume
    (new G4Box( itsName+"_marker_log",
                xLength,
		yLength,
		(itsLength+BDSGlobalConstants::Instance()->GetLengthSafety())/2), //z half length 
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     itsName+"_log");

      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
        BuildTunnel();
      }
      BuildInnerCollimator();
      BuildBLMs();

      itsSolidLogVol->SetVisAttributes(itsVisAttributes);

      //visual attributes      
      G4VisAttributes* VisAtt1 =
        new G4VisAttributes(G4Colour(0., 0., 0.));
      VisAtt1->SetForceSolid(true);
      if (itsInnerLogVol) itsInnerLogVol->SetVisAttributes(VisAtt1);

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }  
}

void BDSCollimator::BuildBLMs(){
  itsBlmLocationR=itsOuterR*2;
  BDSAcceleratorComponent::BuildBLMs();
}

G4VisAttributes* BDSCollimator::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceSolid(true);
  return itsVisAttributes;
}


void BDSCollimator::BuildInnerCollimator()
{

  // zero aperture --> no aperture
  if(itsXAper <= 0) itsXAper = DBL_MIN;//BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  if(itsYAper <= 0) itsYAper = DBL_MIN;//BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;

  if( (itsXAper>0) && (itsYAper>0) ){
#ifdef BDSDEBUG
    G4cout << "BDSCollimator: building aperture" << G4endl;
#endif
    if(itsType == "rcol")
      {
	itsInnerSolid=new G4Box(itsName+"_inner",
				itsXAper,
				itsYAper,
				itsLength/2);
      }
    
    if(itsType == "ecol")
      {
	itsInnerSolid=new G4EllipticalTube(itsName+"_inner",
					   itsXAper,
					   itsYAper,
					   itsLength/2);
      }
  
    itsInnerLogVol=
      new G4LogicalVolume(itsInnerSolid,
			  BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			  itsName+"_inner_log");

#ifndef NOUSERLIMITS
  itsInnerLogVol-> SetUserLimits(itsUserLimits);
#endif

  }
  
  itsOuterSolid = new G4Box(itsName+"_outer_solid",
			    itsOuterR,
			    itsOuterR,
			    itsLength/2);

  G4ThreeVector nullThreeVector = G4ThreeVector(0,0,0);
  G4RotationMatrix *nullRotationMatrix = new G4RotationMatrix();  

  itsSolidLogVol=
    new G4LogicalVolume(itsOuterSolid,
                        itsCollimatorMaterial,
			itsName+"_solid_log");
  
#ifndef NOUSERLIMITS
  itsSolidLogVol-> SetUserLimits(itsUserLimits);
  itsMarkerLogicalVolume->SetUserLimits(itsUserLimits);
#endif
  itsPhysiComp = 
    new G4PVPlacement(
		      nullRotationMatrix,   // no rotation
		      nullThreeVector,        // its position
		      itsSolidLogVol,    // its logical volume
		      itsName+"_solid_phys",	     // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,		     // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		     // copy number  

  if( (itsXAper>0) && (itsYAper>0) ){
#ifdef BDSDEBUG
    G4cout << "BDSCollimator: placing aperture" << G4endl;
#endif
    itsPhysiComp2 = 
      new G4PVPlacement(
			nullRotationMatrix,  // no rotation
			nullThreeVector,     // its position
			itsInnerLogVol,      // its logical volume
			itsName+"_inner_phys", // its name
			itsSolidLogVol,      // its mother  volume
			false,		   // no boolean operation
			0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		   // copy number 
    SetMultiplePhysicalVolumes(itsPhysiComp2);
  } 
  
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    SetSensitiveVolume(itsSolidLogVol);
  }
  SetMultiplePhysicalVolumes(itsPhysiComp);
#ifdef BDSDEBUG
  G4cout << "BDSCollimator: finished building geometry" << G4endl;
#endif
}


BDSCollimator::~BDSCollimator()
{
}
