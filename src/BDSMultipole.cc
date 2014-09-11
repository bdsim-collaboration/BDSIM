/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added extra parameter to BuildLogicalVolume so that it is 
     possible to set the material as either Iron or Vacuum
   Removed StringFromInt function
*/


#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <map>

#include "BDSMultipole.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4Trap.hh"
#include "G4EllipticalTube.hh"
#include "G4Cons.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "G4MagIntegratorStepper.hh"

#include "BDSMaterials.hh"
#include "BDSMultipoleOuterMagField.hh"
#include "G4MagneticField.hh"

#include <string>

//============================================================
BDSMultipole::BDSMultipole( G4String aName, 
			    G4double aLength,
			    G4double aBpRadius,
			    G4double aInnerIronRadius,
                            G4VisAttributes* aVisAtt,
			    std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                            G4String aTunnelMaterial,
                            G4String aMaterial,
			    G4double aXAper,
			    G4double aYAper,
			    G4double angle,
			    G4double tunnelRadius,
			    G4double tunnelOffsetX,
			    G4double phiAngleIn,
			    G4double phiAngleOut,
			    G4bool beampipeThicknessSet,
			    G4double beampipeThickness):
  BDSAcceleratorComponent(
			 aName, 
			 aLength,
			 aBpRadius,
			 aXAper,
			 aYAper,
			 aVisAtt,
			 blmLocZ,
			 blmLocTheta,
                         aTunnelMaterial,
			 aMaterial,
			 angle,
			 0.,
			 0.,
			 0.,
			 tunnelRadius,
			 tunnelOffsetX),
  itsInnerIronRadius(aInnerIronRadius)
{
  ConstructorInit();
  itsPhiAngleIn=phiAngleIn;
  itsPhiAngleOut=phiAngleOut;
  SetBeampipeThickness(beampipeThicknessSet, beampipeThickness); 
}

BDSMultipole::BDSMultipole( G4String aName, 
			    G4double aLength,
			    G4double aBpRadius,
			    G4double aInnerIronRadius,
                            G4VisAttributes* aVisAtt,
                            G4String aTunnelMaterial,
                            G4String aMaterial,
			    G4double aXAper,
			    G4double aYAper,
			    G4double angle,
			    G4double tunnelRadius,
			    G4double tunnelOffsetX,
			    G4bool beampipeThicknessSet,
			    G4double beampipeThickness):
  BDSAcceleratorComponent(
			 aName, 
			 aLength,
			 aBpRadius,
			 aXAper,
			 aYAper,
			 aVisAtt,
                         aTunnelMaterial,
			 aMaterial,
			 angle,
			 0.,
			 0.,
			 0.,
			 tunnelRadius,
			 tunnelOffsetX),
  itsInnerIronRadius(aInnerIronRadius)
{
  ConstructorInit();
  SetBeampipeThickness(beampipeThicknessSet, beampipeThickness); 
}

void BDSMultipole::ConstructorInit(){
  CalculateLengths();

  itsStepper=NULL;
  itsMagField=NULL;
  itsEqRhs=NULL;

  itsBeampipeLogicalVolume=NULL;
  itsInnerBPLogicalVolume=NULL;

  itsBeampipeUserLimits=NULL;
  itsPhysiComp=NULL; 
  itsPhysiInner=NULL;
  itsBPFieldMgr=NULL;
  itsOuterFieldMgr=NULL;

  itsBeampipeSolid=NULL;
  itsInnerBeampipeSolid=NULL;

  itsChordFinder=NULL;
  itsOuterMagField=NULL;
}

void BDSMultipole::SetBeampipeThickness(G4bool set, G4double val){
  if(set){
    itsBeampipeThickness=val;
  } else {
    itsBeampipeThickness=BDSGlobalConstants::Instance()->GetBeampipeThickness();
  }
}

void BDSMultipole::Build()
{
  //
  // build beampipe (geometry + magnetic field)
  //
  BuildBPFieldAndStepper();
  BuildBPFieldMgr(itsStepper, itsMagField);
  BuildMarkerLogicalVolume();
  BuildOuterLogicalVolume();
  BuildBeampipe();
  if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
    BuildTunnel();
  }
  //Build the beam loss monitors
  BuildBLMs();
}

void BDSMultipole::BuildBLMs(){
  itsBlmLocationR=itsOuterR;
  BDSAcceleratorComponent::BuildBLMs(); // resets itsBlmLocationR! -- JS
}

void BDSMultipole::BuildBeampipe(G4String materialName)
{
  // build beampipe
  G4RotationMatrix* RotY = NULL; // no rotation

  if((itsPhiAngleIn==0)&&(itsPhiAngleOut==0)){
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "Building ordinary beam pipe (not trapezoid) " << G4endl;
#endif

#ifdef BDSDEBUG 
    G4cout << __METHOD_NAME__ << "Outer pipe :"
	   << " r= " << itsBpRadius/CLHEP::m << " m"
	   << " l= " << itsLength/(2.)/CLHEP::m << " m"
	   << G4endl;
    G4cout << __METHOD_NAME__ << "Drift aperX: " << this->GetAperX()/CLHEP::m << " m" << G4endl;
    G4cout << __METHOD_NAME__ << "Drift aperY: " << this->GetAperY()/CLHEP::m << " m" << G4endl;
#endif
    
    
  G4EllipticalTube* tmp_tube =new G4EllipticalTube(itsName+"_bmp_solid_tmp",
						   this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness(),
						   this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness(),
						   itsLength/2);
  
  itsInnerBeampipeSolid=new G4EllipticalTube(itsName+"_inner_bmp_solid",
					     this->GetAperX(),
					     this->GetAperY(),
					     itsLength/2);

  G4EllipticalTube* largerInnerBeampipeSolid_tmp=new G4EllipticalTube(itsName+"larger_inner_bmp_solid",
								      this->GetAperX()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
								      this->GetAperY()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
								      itsLength);
  
  itsBeampipeSolid = new G4SubtractionSolid(itsName + "_bmp_solid",
					    tmp_tube,
                              
                                            largerInnerBeampipeSolid_tmp);
  
  } else {
    RotY=BDSGlobalConstants::Instance()->RotY90();
    //
    // compute some geometrical parameters
    //
    G4double boxSize = BDSGlobalConstants::Instance()->GetComponentBoxSize();
  
    G4double xHalfLengthPlus, xHalfLengthMinus, tubLen;
    xHalfLengthPlus = (itsLength + (boxSize/2.0)*(tan(itsPhiAngleIn) -tan(itsPhiAngleOut)))/2.0;
    xHalfLengthMinus = (itsLength +  (boxSize/2.0)*(tan(itsPhiAngleOut)-tan(itsPhiAngleIn )))/2.0;
    tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  
    if((xHalfLengthPlus<0) || (xHalfLengthMinus<0)){
      G4cerr << "Bend radius in " << itsName << " too small for this tunnel/component geometry. Exiting." << G4endl;
      exit(1);
    }

    //
    // build beampipe
    //
    G4VSolid *pipeTubsEnv = new G4SubtractionSolid("_pipe_outer_env",
						   new G4EllipticalTube(itsName+"_pipe_outer_tmp_1",
									this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness(), 
									this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness(),          
									tubLen),
						   new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
									this->GetAperX()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, 
									this->GetAperY()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,          
									tubLen*2)
						   );



  
    G4VSolid *pipeInnerEnv = new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
						  this->GetAperX(), 
						  this->GetAperY(),          
						  tubLen);
  
    itsBeampipeSolid =
      new G4IntersectionSolid(itsName+"_pipe_outer",
			      pipeTubsEnv,
			      itsMarkerSolidVolume,
			      BDSGlobalConstants::Instance()->RotYM90(),
			      (G4ThreeVector)0);
  
    itsInnerBeampipeSolid =
      new G4IntersectionSolid(itsName+"_pipe_inner",
			      pipeInnerEnv, 
			      itsMarkerSolidVolume,
			      BDSGlobalConstants::Instance()->RotYM90(),
			      (G4ThreeVector)0);
  
  }
  FinaliseBeampipe(materialName,RotY);
}

void BDSMultipole::BuildBeampipe(G4double startAper,
                                 G4double endAper, G4String materialName)
{
  //make sure length is not 0 
  if (itsLength==0){
    itsLength=itsLength+pow(1.0,-20.0);
  }

  G4RotationMatrix* RotY=NULL ;
  
  // build beampipe
  
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Outer pipe :"
	 << " start r= " << startAper/CLHEP::m << " m"
	 << " end r= " << endAper/CLHEP::m << " m"
	 << " l= " << itsLength/(2.)/CLHEP::m << " m"
	 << G4endl;
#endif
  
  itsBeampipeSolid=new G4Cons(itsName+"_bmp_solid",
			      startAper,
			      startAper+BDSGlobalConstants::Instance()->GetBeampipeThickness(),
			      endAper,
			      endAper+BDSGlobalConstants::Instance()->GetBeampipeThickness(),
			      itsLength/(2.),
			      0,CLHEP::twopi*CLHEP::radian);
      
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Inner pipe :"
	 << " r= " << (itsBpRadius-BDSGlobalConstants::Instance()->GetBeampipeThickness() )/CLHEP::m << " m"
	 << " l= " << itsLength/(2.)/CLHEP::m << " m"
	 << G4endl;
#endif
      
  itsInnerBeampipeSolid=new G4Cons(itsName+"_inner_bmp_solid",
				   0.,
				   startAper,
				   0.,
				   endAper,
				   itsLength/2-BDSGlobalConstants::Instance()->GetLengthSafety(),
				   0,CLHEP::twopi*CLHEP::radian);
  
  FinaliseBeampipe(materialName,RotY);
}

void BDSMultipole::FinaliseBeampipe(G4String materialName, G4RotationMatrix* RotY) {
  G4Material* material;
  if(materialName == "")
    {
      material = BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName() );
    }
  else
    {
      material = BDSMaterials::Instance()->GetMaterial(materialName);
    }
  
  itsBeampipeLogicalVolume=	
    new G4LogicalVolume(itsBeampipeSolid,
			material,
			itsName+"_bmp_log");
  
  itsInnerBPLogicalVolume=	
    new G4LogicalVolume(itsInnerBeampipeSolid,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_inner_bmp_log");
  
  itsPhysiInner = new G4PVPlacement(
				    RotY,                     // rotation
				    (G4ThreeVector)0,         // at (0,0,0)
				    itsInnerBPLogicalVolume,  // its logical volume
				    itsName+"_inner_bmp_phys",// its name
				    itsMarkerLogicalVolume,   // its mother  volume
				    false,                    // no boolean operation
				    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number
 

  itsPhysiComp = new G4PVPlacement(
				   RotY,                      // rotation
				   (G4ThreeVector)0,          // at (0,0,0)
				   itsBeampipeLogicalVolume,  // its logical volume
				   itsName+"_bmp_phys",	      // its name
				   itsMarkerLogicalVolume,    // its mother  volume
				   false,                     // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number

  //Add the physical volumes to a vector which can be used for e.g. geometrical biasing
  SetMultiplePhysicalVolumes(itsPhysiInner);
  SetMultiplePhysicalVolumes(itsPhysiComp);
  //
  // define sensitive volumes for hit generation
  //
  if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
    AddSensitiveVolume(itsBeampipeLogicalVolume);
  }
  
#ifndef NOUSERLIMITS
  itsBeampipeUserLimits = new G4UserLimits("beampipe cuts");
  itsInnerBeampipeUserLimits = new G4UserLimits("inner beampipe cuts");
  
  G4double maxStepFactor=0.5;

  itsBeampipeUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsBeampipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsBeampipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);
  
  G4double maxStepFactorIn=0.5;

  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength*maxStepFactorIn);
  itsInnerBeampipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsInnerBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsInnerBPLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
#endif
  
  itsBeampipeLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  itsInnerBPLogicalVolume->SetFieldManager(itsBPFieldMgr,false) ;


  // now protect the fields inside the marker volume by giving the
  // marker a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
  
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  
  //
  // set visualization attributes
  //
  G4VisAttributes* VisAtt =  new G4VisAttributes(G4Colour(0., 0., 0));
  VisAtt->SetForceSolid(true);
  VisAtt->SetVisibility(false);
  itsInnerBPLogicalVolume->SetVisAttributes(VisAtt);

  G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  VisAtt1->SetForceSolid(true);
  VisAtt1->SetVisibility(true);
  itsBeampipeLogicalVolume->SetVisAttributes(VisAtt1);
}

void BDSMultipole::BuildBPFieldMgr(G4MagIntegratorStepper* aStepper,
				   G4MagneticField* aField)
{
  itsChordFinder= 
    new G4ChordFinder(aField,
		      BDSGlobalConstants::Instance()->GetChordStepMinimum(),
		      aStepper);

  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsBPFieldMgr= new G4FieldManager();
  itsBPFieldMgr->SetDetectorField(aField);
  itsBPFieldMgr->SetChordFinder(itsChordFinder);
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0){
    itsBPFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  }
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>0)
    itsBPFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0)
    itsBPFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0)
    itsBPFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
}


void BDSMultipole::BuildMarkerLogicalVolume()
{
  if ((itsPhiAngleIn==0)&&(itsPhiAngleOut==0)){
    itsMarkerSolidVolume = new G4Box( itsName+"_marker_solid",
				      itsXLength,
				      itsYLength,
				      itsLength/2);
  } else {
    G4double xLength, yLength;
    xLength = yLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
    xLength = std::max(xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
    yLength = std::max(yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );
    
    G4double transverseSize=std::max(xLength, yLength);
    G4double xHalfLengthPlus, xHalfLengthMinus;
    
    
    xHalfLengthPlus = (itsLength + (transverseSize/2.0)*(tan(itsPhiAngleIn) -tan(itsPhiAngleOut)))/2.0;
    xHalfLengthMinus = (itsLength +  (transverseSize/2.0)*(tan(itsPhiAngleOut)-tan(itsPhiAngleIn )))/2.0;
    
    /*
      if (itsPhiAngleIn >0){
      xHalfLengthPlus = ( (itsLength/itsPhiAngleIn) * sin(itsPhiAngleIn/2.0) 
      + fabs(cos(itsPhiAngleIn/2.0))*(transverseSize/2.0)*(tan(itsPhiAngleIn) -tan(itsPhiAngleOut)))/2.0;
      
      xHalfLengthMinus = ((itsLength/itsPhiAngleIn)*sin(itsPhiAngleIn/2.0)
      + fabs(cos(itsPhiAngleIn/2.0))*(transverseSize/2.0)*(tan(itsPhiAngleOut)-tan(itsPhiAngleIn )))/2.0;
      } else {
      xHalfLengthPlus = (itsLength + (transverseSize/2.0)*(tan(itsPhiAngleIn) -tan(itsPhiAngleOut)))/2.0;
      xHalfLengthMinus = (itsLength +  (transverseSize/2.0)*(tan(itsPhiAngleOut)-tan(itsPhiAngleIn )))/2.0;
      }
    */

    if((xHalfLengthPlus<0) || (xHalfLengthMinus<0)){
      G4cerr << "Bend radius in " << itsName << " too small for this tunnel/component geometry. Exiting." << G4endl;
      exit(1);
    }

    /*  
	itsMarkerSolidVolume = new G4Trd(itsName+"_marker",
	xHalfLengthPlus,     // x hlf lgth at +z
	xHalfLengthMinus,    // x hlf lgth at -z
	transverseSize/2,           // y hlf lgth at +z
	transverseSize/2,           // y hlf lgth at -z
	fabs(cos(itsAngle/2))*transverseSize/2);// z hlf lgth
    */
    
    G4cout << "BDSMultipole::MakeDefaultMarkerLogicalVolume> Trap parameters:  " << G4endl;
    G4cout  <<   
      //fabs(cos(itsPhiAngleIn/2))*transverseSize/2 << " " <<
      transverseSize/2 << " " <<
      itsPhiAngleOut-itsPhiAngleIn << " " <<
      0 << " " <<
      transverseSize/2.0 << " " <<
      xHalfLengthPlus << " " <<
      xHalfLengthPlus << " " <<
      0 << " " <<
      transverseSize/2.0 << " " <<
      xHalfLengthMinus << " " <<
      xHalfLengthMinus << " " <<
      0 << " " << G4endl;
    
    itsMarkerSolidVolume = new G4Trap(itsName+"_trapezoid_marker",
				      //			    fabs(cos(itsPhiAngleIn/2))*transverseSize/2,// z hlf lgth
				      transverseSize/2.0, // z hlf lgth Dz
				      atan((tan(itsPhiAngleOut)-tan(itsPhiAngleIn))/2.0), // pTheta
				      0,// pPhi
				      transverseSize/2.0, // pDy1
				      xHalfLengthPlus,    // pDx1
				      xHalfLengthPlus,    // pDx2
				      0, // pAlp1
				      transverseSize/2.0,  // pDy2
				      xHalfLengthMinus,     // pDx3
				      xHalfLengthMinus,     // pDx4
				      0); // pAlp2
    
  }
  
  
  itsMarkerLogicalVolume=new G4LogicalVolume
    (
     itsMarkerSolidVolume,
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     itsName+"_log");
  
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsMarkerUserLimits =  new G4UserLimits();
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
}


void BDSMultipole::BuildOuterLogicalVolume(G4bool OuterMaterialIsVacuum)
{
  G4Material* material;
  if(itsMaterial != "") material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else material = BDSMaterials::Instance()->GetMaterial("Iron");

  G4double outerRadius = itsOuterR;
  if(itsOuterR==0) outerRadius = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;

#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Outer volume inner radius :"
         << " r= " << (itsInnerIronRadius)/CLHEP::m << " m"
         << " l= " << itsLength/2./CLHEP::m << " m"
         << G4endl;
#endif

#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Outer radius :"
         << " r= " << outerRadius/CLHEP::m << " m"
         << " l= " << itsLength/2./CLHEP::m << " m"
         << G4endl;
#endif

  if(OuterMaterialIsVacuum){
    material=  BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());
  }
   

  itsOuterLogicalVolume=
    new G4LogicalVolume(  new G4SubtractionSolid(itsName+"_outer_solid",
                                                 new G4Tubs(itsName+"_outer_solid_tmp_1",
                                                            itsInnerIronRadius+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
                                                            outerRadius,
                                                            itsLength/2-BDSGlobalConstants::Instance()->GetLengthSafety(),
                                                            0,CLHEP::twopi*CLHEP::radian),
                                                 new G4EllipticalTube(itsName+"_outer_solid_tmp_2",
                                                                      this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
                                                                      this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,
                                                                      itsLength)
                                                 ),
                          material,
                          itsName+"_outer_log");
  
  
  itsPhysiComp = new G4PVPlacement(
				   (G4RotationMatrix*)0,		      // no rotation
				   (G4ThreeVector)0,                      // its position
		      itsOuterLogicalVolume,  // its logical volume
		      itsName+"_outer_phys",  // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,		      // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		      // copy number
  
  //Add the physical volumes to a vector which can be used for e.g. geometrical biasing
  SetMultiplePhysicalVolumes(itsPhysiComp);
  //
  // define sensitive volumes for hit generation
  //
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    AddSensitiveVolume(itsOuterLogicalVolume);
  }

  //
  // set visualization attributes
  //
  itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsOuterUserLimits =  new G4UserLimits("multipole cut");
  itsOuterUserLimits->SetUserMinEkine( BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

void BDSMultipole::BuildOuterFieldManager(G4int nPoles, G4double poleField,
					  G4double phiOffset)
{
  if(nPoles<=0 || nPoles>10 || nPoles%2 !=0)
    G4Exception("BDSMultipole: Invalid number of poles", "-1", FatalException, "");
  itsOuterFieldMgr=NULL;
  if (poleField==0) return;

  itsOuterMagField=new BDSMultipoleOuterMagField(nPoles,poleField,phiOffset);

  itsOuterFieldMgr=new G4FieldManager(itsOuterMagField);
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0){
    itsOuterFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  }
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>0)
    itsOuterFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0)
    itsOuterFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0)
    itsOuterFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
  itsOuterLogicalVolume->SetFieldManager(itsOuterFieldMgr,false);
}


BDSMultipole::~BDSMultipole()
{
  delete itsBPFieldMgr;
  delete itsChordFinder;
#ifndef NOUSERLIMITS
  delete itsOuterUserLimits;
  delete itsMarkerUserLimits;
  delete itsBeampipeUserLimits;
#endif
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
