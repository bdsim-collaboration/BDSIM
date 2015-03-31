/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added extra parameter to BuildLogicalVolume so that it is 
     possible to set the material as either Iron or Vacuum
   Removed StringFromInt function
*/

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <map>
#include <string>
#include <algorithm> // for std::max


#include "G4Box.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagneticField.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetType.hh"
#include "BDSMultipole.hh"
#include "BDSMultipoleOuterMagField.hh"

BDSMultipole::BDSMultipole(BDSMagnetType      type,
			   G4String           name,
			   G4double           length,
			   BDSBeamPipeInfo    info,
			   BDSMagnetOuterInfo magnetOuterInfo,
			   BDSTunnelInfo      tunnelInfo):
  BDSAcceleratorComponent(name,
			  length,
			  0,              //beampipe radius in AC
			  0,0,            //aperx apery
			  "",             //tunnel material name
			  "",             //outer material name
			  0,              //angle
			  0,0,0,          // ???
			  tunnelInfo.aper1,
			  tunnelInfo.tunnelOffsetX),
  itsType(type),itsInnerIronRadius(0), beamPipeType(info.beamPipeType),
  aper1(info.aper1), aper2(info.aper2), aper3(info.aper3), aper4(info.aper4),
  vacuumMaterial(info.vacuumMaterial), beamPipeThickness(info.beamPipeThickness),
  beamPipeMaterial(info.beamPipeMaterial), boxSize(magnetOuterInfo.outerDiameter),
  itsMagnetOuterInfo(magnetOuterInfo)
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

  beampipe = NULL;
  outer    = NULL;
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
  BuildOuterVolume();
  
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

void BDSMultipole::BuildBeampipe()
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
  if(itsBPFieldMgr)
    {beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);}

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
  // LN - I don't think this works as itsPhiAngleIn Out not set properly
  // can revist when redo markervolumes without trapezoid
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

  //update extents - remember we don't know here if the outer volume is built
  //so this can be overwritten but acts as a minimum
  SetExtentX(beampipe->GetExtentX());
  SetExtentY(beampipe->GetExtentY());
  SetExtentZ(beampipe->GetExtentZ());
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

void BDSMultipole::BuildOuterVolume()
{
  G4Material* outerMaterial          = itsMagnetOuterInfo.outerMaterial;
  BDSMagnetGeometryType geometryType = itsMagnetOuterInfo.geometryType;
  G4double outerDiameter             = itsMagnetOuterInfo.outerDiameter;

  //build the right thing depending on the magnet type
  //saves basically the same funciton in each derived class
  BDSMagnetOuterFactory* theFactory = BDSMagnetOuterFactory::Instance();
  switch(itsType.underlying()){
  case BDSMagnetType::decapole:
    outer = theFactory->CreateDecapole(geometryType,itsName,itsLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::vkicker:
    outer = theFactory->CreateKicker(geometryType,itsName,itsLength,beampipe,
				     outerDiameter,true,outerMaterial);
    break;
  case BDSMagnetType::hkicker:
    outer = theFactory->CreateKicker(geometryType,itsName,itsLength,beampipe,
				     outerDiameter,false,outerMaterial);
    break;
  case BDSMagnetType::muspoiler:
    outer = theFactory->CreateMuSpoiler(geometryType,itsName,itsLength,beampipe,
					outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::octupole:
    outer = theFactory->CreateOctupole(geometryType,itsName,itsLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::quadrupole:
    outer = theFactory->CreateQuadrupole(geometryType,itsName,itsLength,beampipe,
					 outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::rectangularbend:
    outer = theFactory->CreateRectangularBend(geometryType,itsName,itsLength,beampipe,
					      outerDiameter,itsAngle,outerMaterial);
    break;
  case BDSMagnetType::rfcavity:
    outer = theFactory->CreateRfCavity(geometryType,itsName,itsLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::sectorbend:
    outer = theFactory->CreateSectorBend(geometryType,itsName,itsLength,beampipe,
					 outerDiameter,itsAngle,outerMaterial);
    break;
  case BDSMagnetType::sextupole:
    outer = theFactory->CreateSextupole(geometryType,itsName,itsLength,beampipe,
					outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::solenoid:
    outer = theFactory->CreateSolenoid(geometryType,itsName,itsLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::multipole:
    outer = theFactory->CreateMultipole(geometryType,itsName,itsLength,beampipe,
					outerDiameter,outerMaterial);
    break;
  default:
    G4cout << __METHOD_NAME__ << "unknown magnet type - no outer volume built" << G4endl;
    outer = NULL;
    break;
  }

  if(outer)
    {
      // register logical volumes using geometry component base class
      RegisterLogicalVolumes(outer->GetAllLogicalVolumes());
  
      // y rotation if using trapezoid marker volume for angled faces
      // LN - I don't think this works as itsPhiAngleIn Out not set properly
      // can revist when redo markervolumes without trapezoid
      G4RotationMatrix* RotY = NULL;
      if ( (fabs(itsPhiAngleIn) > 0) || (fabs(itsPhiAngleOut)>0) )
	{RotY=BDSGlobalConstants::Instance()->RotY90(); }
      
      // place outer volume
      itsPhysiComp = new G4PVPlacement(RotY,                        // rotation
				       outer->GetPlacementOffset(), // at normally (0,0,0)
				       outer->GetContainerLogicalVolume(), // its logical volume
				       itsName+"_outer_phys",       // its name
				       itsMarkerLogicalVolume,      // its mother  volume
				       false,                       // no boolean operation
				       0, BDSGlobalConstants::Instance()->GetCheckOverlaps());// copy number

      //update extents
      SetExtentX(outer->GetExtentX());
      SetExtentY(outer->GetExtentY());
      SetExtentZ(outer->GetExtentZ());
    }
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
    SetExtentX(-transverseSize*0.5,transverseSize*0.5);
    SetExtentY(-transverseSize*0.5,transverseSize*0.5);
    SetExtentZ(-itsLength*0.5,itsLength*0.5);
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
  if (BDSExecOptions::Instance()->GetVisDebug()) {
    itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
  } else {
    itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  }
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

void BDSMultipole::SetVisAttributes()
{
  return;
}
