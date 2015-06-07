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

#include "G4UserLimits.hh"
#include <map>

//============================================================

BDSCollimator::BDSCollimator (G4String aName,G4double aLength,G4double bpRad,
			      G4double xAper,G4double yAper,
			      G4Material *CollimatorMaterial, G4double outR, 
                              std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                              G4String aTunnelMaterial):
  BDSAcceleratorComponent(aName,
			  aLength,bpRad,xAper,yAper,
			  blmLocZ, blmLocTheta, aTunnelMaterial),
  itsPhysiComp(NULL), itsPhysiComp2(NULL), itsSolidLogVol(NULL),
  itsInnerLogVol(NULL), itsInnerSolid(NULL), itsOuterSolid(NULL),
  itsCollimatorMaterial(CollimatorMaterial), itsOuterR(outR)
{
  if(itsOuterR==0) itsOuterR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
}

void BDSCollimator::Build()
{
  BDSAcceleratorComponent::Build();
  BuildBLMs();
}

void BDSCollimator::BuildMarkerLogicalVolume()
{
  G4double xLength, yLength;
  xLength = yLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);

  itsMarkerLogicalVolume=new G4LogicalVolume
    (new G4Box( itsName+"_marker_log",
                xLength,
		yLength,
		(itsLength+BDSGlobalConstants::Instance()->GetLengthSafety())/2), //z half length 
     BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial()),
     itsName+"_log");

  BuildInnerCollimator();

  itsSolidLogVol->SetVisAttributes(itsVisAttributes);

  if (itsInnerLogVol) {
    //visual attributes      
    static G4VisAttributes* VisAtt1 =
      new G4VisAttributes(G4Colour(0., 0., 0.));
    itsInnerLogVol->SetVisAttributes(VisAtt1);
  }
}

void BDSCollimator::BuildBLMs(){
  itsBlmLocationR=itsOuterR*2;
  BDSAcceleratorComponent::BuildBLMs();
}

void BDSCollimator::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
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
	
	/*
	double zPlanepos [2] = {0,itsLength};	
	double rOuter [2] = {itsXAper, itsXAper};
	G4double rInner [2] = {0.0,0.0};

	itsInnerSolid = new G4Polyhedra(itsName+"_inner",
					0.,
					2*CLHEP::pi,
					4,
					2,
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
    RegisterSensitiveVolume(itsSolidLogVol);
  }
  SetMultiplePhysicalVolumes(itsPhysiComp);
#ifdef BDSDEBUG
  G4cout << "BDSCollimator: finished building geometry" << G4endl;
#endif
}


BDSCollimator::~BDSCollimator()
{
}

