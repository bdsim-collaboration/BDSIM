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
#include "G4Polyhedra.hh"
#include "G4Cons.hh"
#include "G4Trd.hh"

#include "G4UnionSolid.hh"

#include "G4UserLimits.hh"
#include <map>

//============================================================
typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSCollimator::BDSCollimator (G4String aName,G4double aLength,G4double bpRad,
			      G4double xAper,G4double yAper,
			      G4Material *CollimatorMaterial, G4double outR, 
                              std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                              G4String aTunnelMaterial):
  BDSAcceleratorComponent(aName,
			  aLength,bpRad,xAper,yAper,
			  blmLocZ, blmLocTheta, aTunnelMaterial),
  itsPhysiComp(NULL), itsPhysiComp2(NULL), itsSolidLogVol(NULL), itsTempSolidLogVol(NULL),
  itsInnerLogVol(NULL), itsInnerSolid(NULL), itsOuterSolid(NULL), itsSolid(NULL), itsSoilTube(NULL),
  itsTunnelTube(NULL),  itsInnerTunnelTube(NULL), itsInnerTunnelLogicalVolume(NULL),
  itsSoilTunnelLogicalVolume(NULL), itsTunnelUserLimits(NULL), itsSoilTunnelUserLimits(NULL),
  itsInnerTunnelUserLimits(NULL), itsEqRhs(NULL),
  itsCollimatorMaterial(CollimatorMaterial), itsOuterR(outR)
{
  if(itsOuterR==0) itsOuterR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
}

void BDSCollimator::Build()
{
  BDSAcceleratorComponent::Build();
  if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
    BuildTunnel();
  }
  BuildBLMs();
}

void BDSCollimator::BuildMarkerLogicalVolume()
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

  BuildInnerCollimator();

  itsSolidLogVol->SetVisAttributes(itsVisAttributes);

  //visual attributes      
  G4VisAttributes* VisAtt1 =
    new G4VisAttributes(G4Colour(0., 0., 0.));
  VisAtt1->SetForceSolid(true);
  if (itsInnerLogVol) itsInnerLogVol->SetVisAttributes(VisAtt1);
}

void BDSCollimator::BuildBLMs(){
  itsBlmLocationR=itsOuterR*2;
  BDSAcceleratorComponent::BuildBLMs();
}

void BDSCollimator::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceSolid(true);
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
	
	
	G4Box* itsInnerSolid1 = new G4Box(itsName+"_inner",
				   2.0*itsXAper,
				   itsYAper/3.0,
				   itsLength/2);

	G4Trd* itsInnerSolid2 = new G4Trd (itsName+"_inner",
					  2.0*itsXAper,
					  itsXAper/3.0,
					  itsYAper,
					  itsYAper/3.0,
					  itsLength/10.0);

	G4Trd* itsInnerSolid3 = new G4Trd (itsName+"_inner",
					  itsXAper/3.0,
					  2.0*itsXAper,
					  itsYAper/3.0,
					  itsYAper,
					  itsLength/10.0);

	G4ThreeVector  translation0(0, 0, -itsLength/2.0+itsLength/10.0);
	G4ThreeVector  translation1(0, 0, itsLength/2.0-itsLength/10.0);

	G4UnionSolid*   itsInnerSolid4 = new G4UnionSolid(itsName+"_inner",
					 itsInnerSolid1,
					 itsInnerSolid2,
					 0,
					 translation0);

	itsInnerSolid = new G4UnionSolid(itsName+"_inner",
					 itsInnerSolid4,
					 itsInnerSolid3,
					 0,
					 translation1);
	// LHC collimator (Cone)

	  /*
     
	G4ThreeVector  translation1(0, 0, -itsLength/3.0);
	G4ThreeVector  translation2(0, 0, itsLength/3.0);
	G4ThreeVector  translation0(0, 0, 0);

	G4Cons* itsInnerSolid2 = new G4Cons (itsName+"_inner",
				     0,
				     itsXAper,
				     0,
				     0,
				     itsLength/6.,
				     0,
				     2*CLHEP::pi);
       
	G4Cons* itsInnerSolid3 = new G4Cons (itsName+"_inner",
				     0,
				     0,
				     0,
				     itsXAper,
				     itsLength/6.,
				     0,
				     2*CLHEP::pi);

	G4UnionSolid* itsInnerSolid4 = new G4UnionSolid(itsName+"_inner",
					 itsInnerSolid1,
					 itsInnerSolid2,
					 0,
					 translation1);

	itsInnerSolid = new G4UnionSolid(itsName+"_inner",
					 itsInnerSolid4,
					 itsInnerSolid3,
					 0,
					 translation2);
	
	*/

	/*
	double zPlanepos [4] = {-itsLength/2.0,-itsLength/3.0,itsLength/3.0,itsLength/2.0};	
	double rOuter [4] = {2.0*itsXAper,itsXAper,itsXAper,2.0*itsXAper};
	G4double rInner [4] = {0.0,0.0,0.0,0.0};	

	itsInnerSolid = new G4Polyhedra(itsName+"_inner",
					0.,
					2*CLHEP::pi,
					20,
					4,
					zPlanepos,
					rInner,
					rOuter);
	*/
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
    AddSensitiveVolume(itsSolidLogVol);
  }
  SetMultiplePhysicalVolumes(itsPhysiComp);
#ifdef BDSDEBUG
  G4cout << "BDSCollimator: finished building geometry" << G4endl;
#endif
}


BDSCollimator::~BDSCollimator()
{
}
