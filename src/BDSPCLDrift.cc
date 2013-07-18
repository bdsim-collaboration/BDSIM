/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 

#include "BDSPCLDrift.hh"
#include "BDSMagField.hh"
#include "BDSDriftStepper.hh"
#include "BDSPCLTube.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4CashKarpRKF45.hh"
#include "G4ExactHelixStepper.hh"
#include "G4UnionSolid.hh"

#include <map>

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSPCLDrift::BDSPCLDrift (G4String aName, G4double aLength, 
			  std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta, G4double aperX, G4double aperYUp, G4double aperYDown, G4double aperDy, G4String tunnelMaterial, G4double aper, G4double tunnelRadius, G4double tunnelOffsetX, G4String aMaterial):
  BDSMultipole(aName, aLength, aper, aper, SetVisAttributes(),  blmLocZ, blmLocTheta, tunnelMaterial, aMaterial, aper, aper, 0, tunnelRadius, tunnelOffsetX),
  itsYAperUp(aperYUp), itsYAperDown(aperYDown), itsDyAper(aperDy),
  outer_solid(NULL),inner_solid(NULL),itsOuterBeamPipeLogicalVolume(NULL),
  itsInnerBeamPipeLogicalVolume(NULL),itsPhysiInner(NULL),itsPhysiOuter(NULL),
  itsBeampipeVisAtt(NULL),itsInnerBeampipeVisAtt(NULL),itsBeampipeUserLimits(NULL),
  itsInnerBeampipeUserLimits(NULL),
  itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
  itsType="pcldrift";
  itsXAper=aperX;

  if (!(*LogVolCount)[itsName])
    {
      //
      // build external volume
      // 
      BuildDefaultMarkerLogicalVolume();

      //
      // build beampipe (geometry + magnetic field)
      //
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
        BuildTunnel();
      }
      BuildBpFieldAndStepper();
      BuildBPFieldMgr(itsStepper, itsMagField);
      BuildBeampipe(itsMaterial);
      BuildBLMs();
  
      //
      // define sensitive volumes for hit generation
      //
      if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
	SetMultipleSensitiveVolumes(itsOuterBeamPipeLogicalVolume);	
      }
      
      //
      // append marker logical volume to volume map
      //
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
      
      //
      // use already defined marker volume
      //
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
}

void BDSPCLDrift::BuildBeampipe(G4String materialName){
  G4Material *material;
  if(materialName != ""){
    material = BDSMaterials::Instance()->GetMaterial( materialName );
  } else {
    material = BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName());
  }
  
  // build beampipe

#ifdef DEBUG 
  G4cout << "Outer pipe :"
         << " r= " << itsBpRadius/m << " m"
         << " l= " << itsLength/(2.)/m << " m"
         << G4endl;
  G4cout << "PCLDrift aperX: " << itsXAper/m << " m" << G4endl;
  G4cout << "PCLDrift aperYUp: " << itsYAperUp/m << " m" << G4endl;
  G4cout << "PCLDrift aperYDown: " << itsYAperDown/m << " m" << G4endl;
  G4cout << "PCLDrift Dy: " << itsDyAper/m << " m" << G4endl;
#endif



  G4double ts = BDSGlobalConstants::Instance()->GetLengthSafety()+itsBeampipeThickness/2;

  BDSPCLTube* innerTube = new BDSPCLTube(itsXAper-ts, itsYAperUp-ts, itsYAperDown-ts, itsDyAper, -1, itsLength, itsName+"_inner");

  BDSPCLTube* outerTube = new BDSPCLTube(itsXAper, itsYAperUp, itsYAperDown, itsDyAper, itsBeampipeThickness, itsLength, itsName+"_outer");
  
  //The inner beam pipe solids need to be fused together to form one seamless shape in order to create a vacuum and field without gaps in it.

  inner_solid = innerTube->GetSolid();
  outer_solid = outerTube->GetSolid();

  delete innerTube;
  delete outerTube;

#ifdef DEBUG
  G4cout << "BDSPCLDrift.cc: Making logical..." << G4endl;
#endif

  itsInnerBeamPipeLogicalVolume=	
    new G4LogicalVolume(inner_solid,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_inner_bmp_log");

  itsOuterBeamPipeLogicalVolume=	
    new G4LogicalVolume(outer_solid,
			material,
			itsName+"_outer_bmp_log");
  
  //
  // set visualization attributes
  //
  itsInnerBeampipeVisAtt =  new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
  itsInnerBeampipeVisAtt->SetVisibility(true);
  itsInnerBeampipeVisAtt->SetForceSolid(true);

  itsBeampipeVisAtt = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  itsBeampipeVisAtt->SetForceWireframe(true);
  itsBeampipeVisAtt->SetVisibility(false);

  itsOuterBeamPipeLogicalVolume->SetVisAttributes(itsBeampipeVisAtt);
  itsInnerBeamPipeLogicalVolume->SetVisAttributes(itsInnerBeampipeVisAtt);

#ifdef DEBUG  
  G4cout << "BDSPCLDrift.cc: Placing..." << G4endl;
#endif

  G4ThreeVector threeVector1;
  threeVector1.setY(0);
  itsPhysiInner = new G4PVPlacement(
				    (G4RotationMatrix*)0,		        // no rotation
				    threeVector1,	               
				    itsInnerBeamPipeLogicalVolume,  // its logical volume
				    itsName+"_inner_bmp_phys",// its name
				    itsMarkerLogicalVolume,   // its mother  volume
				    false,		        // no boolean operation
				    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		        // copy number


  
  itsPhysiOuter = new G4PVPlacement(
				    (G4RotationMatrix*)0,		        // no rotation
				    threeVector1,	               
				    itsOuterBeamPipeLogicalVolume,  // its logical volume
				    itsName+"_inner_bmp_phys",// its name
				    itsMarkerLogicalVolume,   // its mother  volume
				    false,		        // no boolean operation
				    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		        // copy number




  //Add the physical volumes to a vector which can be used for e.g. geometrical biasing
  SetMultiplePhysicalVolumes(itsPhysiInner);
  SetMultiplePhysicalVolumes(itsPhysiOuter);

#ifndef NOUSERLIMITS
  itsBeampipeUserLimits =  new G4UserLimits("beampipe cuts");
  itsBeampipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());

  itsInnerBeampipeUserLimits =  new G4UserLimits("inner beampipe cuts");
  itsInnerBeampipeUserLimits->SetUserMinEkine( BDSGlobalConstants::Instance()->GetThresholdCutCharged());

  G4double stepfactor = 5;

  itsBeampipeUserLimits->SetMaxAllowedStep(itsLength*stepfactor);
  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength*stepfactor);

  itsInnerBeamPipeLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
  itsOuterBeamPipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);


#endif

   itsInnerBeamPipeLogicalVolume->SetFieldManager(itsBPFieldMgr,false);
   itsOuterBeamPipeLogicalVolume->SetFieldManager(itsBPFieldMgr,false);

  
  // now protect the fields inside the marker volume by giving the
  // marker a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
  
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  
#ifdef DEBUG
  G4cout << "BDSPCLDrift.cc: Finished making beam pipe..." << G4endl;
#endif
}

void BDSPCLDrift::BuildBpFieldAndStepper(){
    // set up the magnetic field and stepper
  itsMagField=new BDSMagField(); //Zero magnetic field.
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  itsStepper = new BDSDriftStepper(itsEqRhs);
}

G4VisAttributes* BDSPCLDrift::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,1,0)); //useless
  itsVisAttributes->SetVisibility(false);
  itsVisAttributes->SetForceWireframe(true);
  return itsVisAttributes;
}

void BDSPCLDrift::BuildBLMs(){
  BDSAcceleratorComponent::BuildBLMs();
}

BDSPCLDrift::~BDSPCLDrift()
{
  delete itsVisAttributes;
//   delete outer_solid;
//   delete inner_solid;
//   delete itsOuterBeamPipeLogicalVolume;
//   delete itsInnerBeamPipeLogicalVolume;
//   delete itsPhysiOuter;
//   delete itsPhysiInner;
  delete itsBeampipeVisAtt;
  delete itsInnerBeampipeVisAtt;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
