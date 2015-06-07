#include <list>
#include <sstream>
#include <cmath>

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 

#include "BDSAcceleratorComponent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSMaterials.hh"
#include "BDSReadOutGeometry.hh"
#include "BDSTiltOffset.hh"
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

BDSAcceleratorComponent::BDSAcceleratorComponent (
						  G4String& aName,
						  G4double aLength, 
						  G4double aBpRadius,
						  G4double aXAper,
						  G4double aYAper, 
						  G4String /*aTunnelMaterial*/,
						  G4String aMaterial,
						  G4double angle,
						  G4double XOffset, 
						  G4double YOffset,
						  G4double ZOffset, 
						  G4double /*tunnelRadius*/, 
						  G4double /*tunnelOffsetX*/,
						  G4String /*aTunnelCavityMaterial*/,
						  BDSTiltOffset tiltOffsetIn):
  BDSGeometryComponent(NULL,NULL),
  itsName(aName),
  itsLength(aLength),
  itsBpRadius(aBpRadius),
  itsXAper(aXAper),
  itsYAper(aYAper),
  itsAngle(angle),
  itsMaterial(aMaterial),
  itsXOffset(XOffset),
  itsYOffset(YOffset), 
  itsZOffset(ZOffset), 
  tiltOffset(tiltOffsetIn)
{
  ConstructorInit();
}

BDSAcceleratorComponent::BDSAcceleratorComponent(G4String& aName,
						 G4double aLength, 
						 G4double aBpRadius,
						 G4double aXAper,
						 G4double aYAper, 
						 std::list<G4double> blmLocZ, 
						 std::list<G4double> blmLocTheta,
						 G4String /*aTunnelMaterial*/,
						 G4String aMaterial,
						 G4double angle,
						 G4double XOffset, 
						 G4double YOffset,
						 G4double ZOffset, 
						 G4double /*tunnelRadius*/, 
						 G4double /*tunnelOffsetX*/, 
						 G4String /*aTunnelCavityMaterial*/,
						 BDSTiltOffset tiltOffsetIn):
  BDSGeometryComponent(NULL,NULL),
  itsName(aName),
  itsLength(aLength),
  itsBpRadius(aBpRadius),
  itsXAper(aXAper),
  itsYAper(aYAper),
  itsAngle(angle),
  itsMaterial(aMaterial),
  itsBlmLocZ(blmLocZ), 
  itsBlmLocTheta(blmLocTheta),
  itsXOffset(XOffset),
  itsYOffset(YOffset), 
  itsZOffset(ZOffset), 
  tiltOffset(tiltOffsetIn)
{
  if (itsBlmLocZ.size() != itsBlmLocTheta.size()){
    G4cerr << "BDSAcceleratorComponent: error, lists blmLocZ and blmLocTheta are of unequal size" << G4endl;
    G4cerr << "blmLocZ.size() = " << blmLocZ.size() << G4endl;
    G4cerr << "blmLocTheta.size() = " << blmLocTheta.size() << G4endl;
    exit(1);
  }
  ConstructorInit();
}

inline void BDSAcceleratorComponent::ConstructorInit(){
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
  
  CalculateLengths(); // Calculate dimensions based on component and tunnel dimensions

  itsMarkerLogicalVolume=NULL;
  itsMarkerSolidVolume=NULL;
  itsMarkerUserLimits=NULL;

  itsSPos = 0.0;
  itsCopyNumber = 0;
  readOutLV = NULL;
  itsPrecisionRegion=0;
  
  itsOuterStepper=NULL;
  itsOuterUserLimits=NULL;

  itsBLMLogicalVolume=NULL;
  itsBlmCaseLogicalVolume=NULL;
  
  itsVisAttributes=NULL;
  VisAtt=NULL;
  VisAtt1=NULL;
  VisAtt2=NULL;
  itsBLMSolid=NULL;
  itsBlmOuterSolid=NULL;
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
  Build();

  readOutLV = BDS::BuildReadOutVolume(itsName,itsLength,itsAngle);
}

void BDSAcceleratorComponent::RegisterMarkerWithBaseClass()
{
  // Register volumes with base class (BDSGeometryComponent)
  RegisterLogicalVolume(itsMarkerLogicalVolume);

  // Manually assign base class (BDSGeometryComponent) members as constructor used with null pointers
  containerSolid         = itsMarkerSolidVolume;
  containerLogicalVolume = itsMarkerLogicalVolume;
}

void BDSAcceleratorComponent::Build()
{
  SetVisAttributes(); // sets color attributes, virtual method
  BuildMarkerLogicalVolume(); // pure virtual provided by derived class
  RegisterMarkerWithBaseClass();

  // visual attributes
  if(itsMarkerLogicalVolume)
    {
    if (BDSExecOptions::Instance()->GetVisDebug())
      {itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
    else
      {itsMarkerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}
    }
}

void BDSAcceleratorComponent::PrepareField(G4VPhysicalVolume*)
{//do nothing by default
  return;
}

void BDSAcceleratorComponent::CalculateLengths(){
  //all half lengths
  itsXLength = itsYLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
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
         RegisterSensitiveVolume(itsBLMLogicalVolume);
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
