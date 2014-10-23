//  
//   BDSIM, (C) 2001-2006 
//   
//   version 0.3
//  
//
//
//
//
//   Generic accelerator component class
//
//
//   History
//
//     24 Nov 2006 by Agapov,  v.0.3
//     x  x   2002 by Blair
//
//


#include <list>
#include <sstream>
#include <cmath>

#include "BDSGlobalConstants.hh" 

#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4AssemblyVolume.hh"
#include "G4Transform3D.hh"

typedef std::map<G4String,int> LogVolCountMap;
LogVolCountMap* LogVolCount = new LogVolCountMap();

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
LogVolMap* LogVol = new LogVolMap();

BDSAcceleratorComponent::BDSAcceleratorComponent (
						  G4String& aName,
						  G4double aLength, 
						  G4double aBpRadius,
						  G4double aXAper,
						  G4double aYAper, 
						  G4String aTunnelMaterial,
						  G4String aMaterial,
						  G4double angle,
						  G4double XOffset, 
						  G4double YOffset,
						  G4double ZOffset, 
						  G4double tunnelRadius, 
						  G4double tunnelOffsetX,
						  G4String aTunnelCavityMaterial):
  itsName(aName),
  itsLength(aLength),
  itsBpRadius(aBpRadius),
  itsXAper(aXAper),
  itsYAper(aYAper),
  itsAngle(angle),
  itsMaterial(aMaterial),
  itsTunnelMaterialName(aTunnelMaterial),
  itsXOffset(XOffset),
  itsYOffset(YOffset), 
  itsZOffset(ZOffset), 
  itsTunnelRadius(tunnelRadius), 
  itsTunnelOffsetX(tunnelOffsetX),
  itsTunnelCavityMaterial(aTunnelCavityMaterial)
{
  ConstructorInit();
}

BDSAcceleratorComponent::BDSAcceleratorComponent (
						  G4String& aName,
						  G4double aLength, 
						  G4double aBpRadius,
						  G4double aXAper,
						  G4double aYAper, 
						  std::list<G4double> blmLocZ, 
						  std::list<G4double> blmLocTheta,
						  G4String aTunnelMaterial,
						  G4String aMaterial,
						  G4double angle,
						  G4double XOffset, 
						  G4double YOffset,
						  G4double ZOffset, 
						  G4double tunnelRadius, 
						  G4double tunnelOffsetX, 
						  G4String aTunnelCavityMaterial):
  itsName(aName),
  itsLength(aLength),
  itsBpRadius(aBpRadius),
  itsXAper(aXAper),
  itsYAper(aYAper),
  itsAngle(angle),
  itsMaterial(aMaterial),
  itsBlmLocZ(blmLocZ), 
  itsBlmLocTheta(blmLocTheta),
  itsTunnelMaterialName(aTunnelMaterial),
  itsXOffset(XOffset),
  itsYOffset(YOffset), 
  itsZOffset(ZOffset), 
  itsTunnelRadius(tunnelRadius), 
  itsTunnelOffsetX(tunnelOffsetX), 
  itsTunnelCavityMaterial(aTunnelCavityMaterial)
{
  if (itsBlmLocZ.size() != itsBlmLocTheta.size()){
    G4cerr << "BDSAcceleratorComponent: error, lists blmLocZ and blmLocTheta are of unequal size" << G4endl;
    G4cerr << "blmLocZ.size() = " << blmLocZ.size() << G4endl;
    G4cerr << "blmLocTheta.size() = " << blmLocTheta.size() << G4endl;
    exit(1);
  }
  ConstructorInit();
}

inline void BDSAcceleratorComponent::ConstructorInit(){\
  nullThreeVector = G4ThreeVector(0,0,0);
  nullRotationMatrix = new G4RotationMatrix();
  //  itsInnerBeampipeUserLimits =new G4UserLimits();
#ifndef NOUSERLIMITS
  itsUserLimits = new G4UserLimits();
  //  itsUserLimits->SetMaxAllowedStep(itsLength);
  itsUserLimits->SetMaxAllowedStep(1*CLHEP::m);
  itsUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
#endif
  itsPhi = 0;
  itsTheta = 0;
  itsPsi = 0;
  itsK1 = 0.0; itsK2 = 0.0; itsK3 = 0.0;
  itsTilt = 0;
  itsPhiAngleIn = 0.0;
  itsPhiAngleOut = 0.0;
  itsOuterR=0;
  itsBlmLocationR=0;
  if (itsTunnelRadius<=BDSGlobalConstants::Instance()->GetLengthSafety()){
    itsTunnelRadius=BDSGlobalConstants::Instance()->GetTunnelRadius();
  }
  CalculateLengths(); // Calculate dimensions based on component and tunnel dimensions
  itsOuterLogicalVolume=NULL;
  itsMarkerLogicalVolume=NULL;
  itsRotation=NULL;
  itsOuterStepper=NULL;
  itsOuterUserLimits=NULL;
  itsMarkerUserLimits=NULL;
  itsInnerBeampipeUserLimits=NULL;
  itsInnerMostLogicalVolume=NULL;
  itsPrecisionRegion=0;
  itsMarkerSolidVolume=NULL;
  itsBLMLogicalVolume=NULL;
  itsBlmCaseLogicalVolume=NULL;
  itsSoilTunnelLogicalVolume=NULL;

  nullRotationMatrix=NULL;
  itsVisAttributes=NULL;
  VisAtt=NULL;
  VisAtt1=NULL;
  VisAtt2=NULL;
  itsBLMSolid=NULL;
  itsBlmOuterSolid=NULL;
  itsSPos = 0.0;
  itsCopyNumber = 0;
}

BDSAcceleratorComponent::~BDSAcceleratorComponent ()
{
  delete itsVisAttributes;
#ifndef NOUSERLIMITS
  delete itsUserLimits;
#endif
  delete VisAtt;
  delete VisAtt1;
  delete VisAtt2;
}

void BDSAcceleratorComponent::Initialise()
{
  /// check and build logical volume

  // set copy number (count starts at 0)
  // post increment guarantees itsCopyNumber starts at 0!
  itsCopyNumber = (*LogVolCount)[itsName]++;
  if(itsCopyNumber == 0)
    {
      Build();
      //
      // append marker logical volume to volume map
      //
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      //
      // use already defined marker volume
      //
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
}

void BDSAcceleratorComponent::Build()
{
  SetVisAttributes(); // sets color attributes, virtual method
  BuildMarkerLogicalVolume(); // pure virtual provided by derived class
}

void BDSAcceleratorTunnel::BuildTunnel(){
  itsTunnel = new BDSTunnel(this);
  //  _tunnel->radius(GetTunnelRadius());
  //  _tunnel->offsetX(GetTunnelOffsetX());
  //  _tunnel->floorBeamlineHeight(GetFloorBeamlineHeight());
  //  _tunnel->beamlineCeilingHeight(GetBeamlineCeilingHeight());
  //  _tunnel->MotherComponent(this);
  itsTunnel->Build();
}

void BDSAcceleratorComponent::PrepareField(G4VPhysicalVolume*)
{//do nothing by default
  return;
}

void BDSAcceleratorComponent::CalculateLengths(){
  itsXLength = itsYLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
  itsXLength = std::max(itsXLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  itsYLength = std::max(itsYLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );
  
}

void BDSAcceleratorComponent::AlignComponent(G4ThreeVector& TargetPos,
					     G4RotationMatrix *TargetRot, 
					     G4RotationMatrix&,
					     G4ThreeVector&,
					     G4ThreeVector&,
					     G4ThreeVector&,
					     G4ThreeVector&,
					     G4ThreeVector&)
{//do nothing by default, except set rotation and position for BDSOutline
  itsRotation=TargetRot;
  itsPosition=TargetPos;
  return;
}



void BDSAcceleratorComponent::BuildBLMs()
{
 if(itsBlmLocZ.size() > 0){

   //declare materials
   G4Material *blmMaterial;
   blmMaterial = BDSMaterials::Instance()->GetMaterial("nitrogen");
   G4Material *caseMaterial;
   caseMaterial = BDSMaterials::Instance()->GetMaterial("nitrogen");
   
   /*
     blmCaseMaterial = BDSMaterials::Instance()->GetMaterial("cement");
   */

   //declare Matrices & doubles
   G4RotationMatrix blmRot = G4RotationMatrix();
   G4ThreeVector blmTrans;
   //    G4double blmXRot;
   G4double blmYTrans;
   G4double blmXTrans;
   G4double blmCaseThickness;
   
   G4AssemblyVolume* assemblyBlms= new G4AssemblyVolume();
   G4Transform3D blmTr3d;
   
   //set case thickness
   blmCaseThickness = 1e-20*CLHEP::m;

   itsBlmLocationR = itsBlmLocationR + BDSGlobalConstants::Instance()->GetBlmRad() + BDSGlobalConstants::Instance()->GetLengthSafety() +itsBpRadius + blmCaseThickness;
   
#ifdef BDSDEBUG
   G4cout << "BDSAcceleratorComponent::BuildBLMs() itsBlmLocationRadius = " << itsBlmLocationR/CLHEP::mm << " mm" << G4endl;
#endif
   
   G4double localLength;
   
   //Make sure BLM length can never exceed component length
#ifdef BDSDEBUG
   G4cout << "BLM length = " << BDSGlobalConstants::Instance()->GetBlmLength()/CLHEP::m << " m" << G4endl;
   G4cout << "BLM radius = " << BDSGlobalConstants::Instance()->GetBlmRad()/CLHEP::m << " m" << G4endl;
   G4cout << "Component length = " << itsLength/CLHEP::m << " m" << G4endl;
#endif
   if(BDSGlobalConstants::Instance()->GetBlmLength()>=(itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())){
     G4cout << itsName << "Warning: BLM longer than element. Setting BLM length to element length." << G4endl;
     localLength = itsLength-BDSGlobalConstants::Instance()->GetLengthSafety();
   } else {
     localLength=BDSGlobalConstants::Instance()->GetBlmLength();
   }
   
   G4int i = 0;
   std::list<double>::iterator zit;
   std::list<double>::iterator thetait;
   for (zit=itsBlmLocZ.begin(), thetait=itsBlmLocTheta.begin(); zit!=itsBlmLocZ.end(); zit++, thetait++){
#ifdef BDSDEBUG
     G4cout << "Building BLM " << i << G4endl; 
#endif
     G4double indexInt = (G4double)i;
     G4String index;
     std::stringstream out;
     out << indexInt;
     index = out.str();
     
     blmYTrans=itsBlmLocationR*cos(*thetait);
     blmXTrans=-1*itsBlmLocationR*sin(*thetait);
     blmTrans.setZ(*zit*1000);
     blmTrans.setY(blmYTrans);
     blmTrans.setX(blmXTrans);
     
#ifdef BDSDEBUG
     if((*zit*1000) < 0){
       G4cout << "itsLength is: " << itsLength << G4endl;
       G4cout << "z position defined is: " << (*zit*1000) << G4endl;
     }
#endif
     
     itsBLMSolid = new G4Tubs(itsName + "_BLM_sol_" + index,
				      0,
				      BDSGlobalConstants::Instance()->GetBlmRad(),
				      localLength/2, 0, CLHEP::twopi*CLHEP::radian);
     
     
     itsBlmOuterSolid = new G4Tubs(itsName + "_Blm_outCase_sol_" + index, 
					   0,
					   BDSGlobalConstants::Instance()->GetBlmRad() + blmCaseThickness,
					   localLength/2 + blmCaseThickness, 0, CLHEP::twopi*CLHEP::radian);
     
     G4SubtractionSolid *itsBlmCaseSolid = new G4SubtractionSolid(itsName + "_blm_case_subsolid_" + index,
								  itsBlmOuterSolid,
								  itsBLMSolid);       
       
       itsBLMLogicalVolume = new G4LogicalVolume(itsBLMSolid,
						 blmMaterial,
						 itsName + "_blm_logvol_" + index);
       
       //Set BLM gas as sensitive if option is chosen
       if(BDSGlobalConstants::Instance()->GetSensitiveBLMs()){
         AddSensitiveVolume(itsBLMLogicalVolume);
       }

       itsBlmCaseLogicalVolume = new G4LogicalVolume(itsBlmCaseSolid,
						     caseMaterial,
						     itsName + "_blm_case_logvol_" + index);
       
       itsBlmCaseLogicalVolume->SetVisAttributes(VisAtt2);
       itsBLMLogicalVolume->SetVisAttributes(VisAtt1);
	 
       blmTr3d=G4Transform3D(blmRot,blmTrans);
     
       assemblyBlms->AddPlacedVolume(itsBLMLogicalVolume,blmTr3d);
       assemblyBlms->AddPlacedVolume(itsBlmCaseLogicalVolume,blmTr3d);
   }  
   if (itsType=="sbend" || itsType=="rbend"){ // should ideally not be here - JS
     blmRot.setTheta(CLHEP::twopi/4.0);
     blmRot.setPsi(-CLHEP::twopi/4.0);
     blmRot.setPhi(CLHEP::twopi/4.0);
   } else {
     blmRot.setTheta(0.0);
     blmRot.setPsi(0.0);
     blmRot.setPhi(0.0);
   }
   blmTrans.setX(0.0);
   blmTrans.setY(0.0);
   blmTrans.setZ(0.0);
   blmTr3d=G4Transform3D(blmRot,blmTrans);
   assemblyBlms->MakeImprint(itsMarkerLogicalVolume,blmTr3d);
 }
}

//This Method is for investigating the Anomalous signal at LHC junction IP8
// no longer used
/*
void BDSAcceleratorComponent::BuildGate()
{
  //Declare variables, matrices and constants to use
  G4RotationMatrix *gateRot = new G4RotationMatrix();
  G4ThreeVector gateTrans;
  G4ThreeVector floorOffsetThreeVector;
  G4Material *gateMaterial;

  //Set basic variables
  G4double gateX = 0.05*CLHEP::m;
  G4double gateY = 1.0*CLHEP::m;
  G4double gateZ = 0.63*0.99*CLHEP::m;//1.5*m;
  G4double gateXOffset = 3*CLHEP::m;
  G4double gateYOffset = -BDSGlobalConstants::Instance()->GetTunnelFloorOffset()/2 + gateY/2;
  G4double gateZOffset = -4.095*CLHEP::m; //Shifted along z 4.095m from centre of dipole
  gateMaterial = BDSMaterials::Instance()->GetMaterial("iron");
  
  //Set position and rotation of gate
  gateTrans.setX(gateZOffset);
  gateTrans.setY(gateYOffset);
  gateTrans.setZ(gateXOffset);

  G4Box* itsGateSolid = new G4Box(itsName + "_gate_Solid",
			      gateZ,
			      gateY,
			      gateX
			      );
  G4LogicalVolume* itsGateLogicalVolume = new G4LogicalVolume(itsGateSolid,
							  gateMaterial,
							  itsName + "_gate_Log_Vol");

  G4PVPlacement*  itsGatePhysi = new G4PVPlacement(gateRot,
						  gateTrans,
						  itsGateLogicalVolume,
						  itsName + "_gate_physi",
						  itsMarkerLogicalVolume,
						  false,
						   0,
						   BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsGatePhysi);
  
  G4VisAttributes* VisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  VisAtt->SetVisibility(true);
  VisAtt->SetForceSolid(true);
  itsGateLogicalVolume->SetVisAttributes(VisAtt);
  }
*/
//  LocalWords:  itsTunnelUserLimits
