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
#include <string>
#include <algorithm> // for std::max

#include "BDSMultipole.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4EllipticalTube.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagneticField.hh"
#include "G4Polyhedra.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSMaterials.hh"
#include "BDSMultipoleOuterMagField.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeType.hh"

//============================================================
BDSMultipole::BDSMultipole( G4String aName, 
			    G4double aLength,
			    G4double aBpRadius,
			    G4double aInnerIronRadius,
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

BDSMultipole::BDSMultipole( G4String        name, 
			    G4double        length,
			    BDSBeamPipeType beamPipeTypeIn,
			    G4double        aper1In,
			    G4double        aper2In,
			    G4double        aper3In,
			    G4double        aper4In,
			    G4Material*     vacuumMaterialIn,
			    G4double        beamPipeThicknessIn,
			    G4Material*     beamPipeMaterialIn,
			    G4double        boxSizeIn,
			    G4String        outerMaterial,
			    G4String        tunnelMaterial,
			    G4double        tunnelRadius,
			    G4double        tunnelOffsetX):
  BDSAcceleratorComponent(name,
			  length,
			  0,              //beampipe radius in AC
			  0,0,            //aperx apery
			  tunnelMaterial,
			  outerMaterial,
			  0,              //angle
			  0,0,0,          // ???
			  tunnelRadius,
			  tunnelOffsetX),
  itsInnerIronRadius(0),beamPipeType(beamPipeTypeIn),aper1(aper1In),aper2(aper2In),
  aper3(aper3In),aper4(aper4In),vacuumMaterial(vacuumMaterialIn),
  beamPipeThickness(beamPipeThicknessIn),beamPipeMaterial(beamPipeMaterialIn),
  boxSize(boxSizeIn)
{
  ConstructorInit();
}

			  
void BDSMultipole::ConstructorInit(){
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
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BuildBPFieldAndStepper();
  BuildBPFieldMgr(itsStepper, itsMagField);

  BDSAcceleratorComponent::Build(); //builds marker logical volume & itVisAttributes
  
  BuildBeampipe();
  BuildOuterLogicalVolume();
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

void BDSMultipole::BuildBeampipe(G4String /*materialName*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(beamPipeType,
							    itsName,
							    itsLength,
							    aper1,
							    aper2,
							    aper3,
							    aper4,
							    vacuumMaterial,
							    beamPipeThickness,
							    beamPipeMaterial);
  BeamPipeCommonTasks();
}

void BDSMultipole::BeamPipeCommonTasks()
{
  //actual beam pipe ->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  // SET FIELD
  beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);
  // now protect the fields inside the marker volume by giving the
  // marker a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
  
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  

  // register logical volumes using geometry component base class
  RegisterLogicalVolumes(beampipe->GetAllLogicalVolumes());


  // y rotation if using trapezoid marker volume for angled faces
  G4RotationMatrix* RotY = NULL;
  if ( (fabs(itsPhiAngleIn) > 0) || (fabs(itsPhiAngleOut)>0) )
    {RotY=BDSGlobalConstants::Instance()->RotY90(); }

  // place beampipe
  itsPhysiComp = new G4PVPlacement(
				   RotY,                      // rotation
				   (G4ThreeVector)0,          // at (0,0,0)
				   beampipe->GetContainerLogicalVolume(),  // its logical volume
				   itsName+"_bmp_phys",	      // its name
				   itsMarkerLogicalVolume,    // its mother  volume
				   false,                     // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number
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
     BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial()),
     itsName+"_log");

  // taken from FinaliseBeamPipe method - supposed to protect against fields being overridden
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);

  // USER LIMITS
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsMarkerUserLimits =  new G4UserLimits();
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif

  // VIS ATTR
#ifdef BDSDEBUG
  itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
#else
  itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
#endif
}


void BDSMultipole::BuildOuterLogicalVolume(G4bool OuterMaterialIsVacuum)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // default cylindrical geometry for straight magnets only
  
  // check if outer volume is required
  if (OuterMaterialIsVacuum)
    {return;} // no need to create another volume
  
  // test beampipe instance exists / has been built already
  if (!beampipe){
    G4cerr << __METHOD_NAME__ << " no beampipe has been built - can't wrap around it" << G4endl;
    exit(1);
  }

  // build the logical volume
  G4Material* material;
  if(itsMaterial != "")
    {material = BDSMaterials::Instance()->GetMaterial(itsMaterial);}
  else
    {material = BDSMaterials::Instance()->GetMaterial("Iron");}
  
  G4double lengthSafety = BDSGlobalConstants::Instance()->GetLengthSafety();
  G4double outerRadius  = boxSize*0.5;
  if (beampipe->ContainerIsCircular())
    {
      // simple circular beampipe - no need for a subtraction solid
      G4double innerRadius = beampipe->GetContainerRadius()+lengthSafety;
      
      // check outerRadius is bigger
      if ((boxSize*0.5) < innerRadius)
	{
	  G4cout << __METHOD_NAME__ << " - warning - beampipe is bigger than the boxSize" << G4endl
		 << "setting boxSize to be just big enough to contain beampipe " << G4endl;
	  outerRadius = innerRadius+1*CLHEP::cm;
	}
      itsOuterLogicalVolume = new G4LogicalVolume( new G4Tubs(itsName+"_outer_solid",
							      innerRadius,
							      outerRadius,
							      (itsLength*0.5)-(2*lengthSafety),
							      0,
							      CLHEP::twopi),
						   material,
						   itsName+"_outer_lv");
     
    }
  else
    {
      // not a simple circular beampipe - have to use subtraction solid
      // check outer radius is really outside the beampipe
      G4double maxX = std::max(beampipe->GetExtentX().first,beampipe->GetExtentX().second);
      G4double maxY = std::max(beampipe->GetExtentY().first,beampipe->GetExtentY().second);
      if (outerRadius < maxX)
	{outerRadius = maxX + 1*CLHEP::cm;} //minimum extra size
      if (outerRadius < maxY)
	{outerRadius = maxY + 1*CLHEP::cm;}
      G4double hypotenuse = sqrt(maxX*maxX + maxY*maxY);
      if (outerRadius < hypotenuse)
	{outerRadius = hypotenuse + 1*CLHEP::cm;}
      
      itsOuterLogicalVolume =
	new G4LogicalVolume( new G4SubtractionSolid (itsName+"_outer_solid",
						     new G4Tubs(itsName+"_outer_solid_cylinder",
								0.0,  // solid cylinder for unambiguous subtraction
								outerRadius,
								itsLength*0.5 - 2.0*lengthSafety, // to ensure it's inside the marker volume
								0,
								CLHEP::twopi),
						     beampipe->GetContainerSubtractionSolid()),
						     material,
						     itsName+"_outer_lv");
    }
  
  RegisterLogicalVolume(itsOuterLogicalVolume);

  // place the outer volume
  itsPhysiComp = new G4PVPlacement(
				   (G4RotationMatrix*)0,   // no rotation
				   (G4ThreeVector)0,       // its position
				   itsOuterLogicalVolume,  // its logical volume
				   itsName+"_outer_phys",  // its name
				   itsMarkerLogicalVolume, // its mother  volume
				   false,		   // no boolean operation
				   0,                      // copy number
				   BDSGlobalConstants::Instance()->GetCheckOverlaps());		      
  
  // set visualization attributes
  itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsOuterUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
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
