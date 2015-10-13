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
//

#include <list>
#include <sstream>
#include <cmath>

#include "BDSGlobalConstants.hh" 

#include "BDSAcceleratorComponent.hh"
#include "BDSMaterials.hh"
#include "BDSDebug.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
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
#include "BDSMagFieldFactory.hh"
#include "G4NystromRK4.hh"

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
						  G4String aTunnelCavityMaterial,
						  G4String bmap,
						  G4double bmapZOffset):
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
  itsBmap(bmap),itsBmapZOffset(bmapZOffset),itsBmapXOffset(0),
  itsChordFinder(NULL), itsStepper(NULL),itsEqRhs(NULL), 
  itsMagField(NULL), itsFieldMgr(NULL)
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
						  G4String aTunnelCavityMaterial,
						  G4String bmap,
						  G4double bmapZOffset):
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
  itsBmap(bmap),itsBmapZOffset(bmapZOffset), itsBmapXOffset(0),
  itsChordFinder(NULL), itsStepper(NULL), itsEqRhs(NULL), 
  itsMagField(NULL), itsFieldMgr(NULL)

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
  itsTunnel=NULL;
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

  nullRotationMatrix=NULL;
  itsVisAttributes=NULL;
  VisAtt=NULL;
  VisAtt1=NULL;
  VisAtt2=NULL;
  itsBLMSolid=NULL;
  itsBlmOuterSolid=NULL;
  itsSPos = 0.0;
  itsCopyNumber = 0;
  _geom=NULL;
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
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSAcceleratorComponent::Build()//Execute build methods common for all classes.
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  SetVisAttributes(); // sets color attributes, virtual method
  BuildMarkerLogicalVolume(); 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << " - setting tunnel mother volume " << G4endl;
#endif
  if(itsTunnel != NULL){
    itsTunnel->motherVolume(GetMarkerLogicalVolume());
  }
#ifdef BDSDEBUG 
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSAcceleratorComponent::BuildStraightMarkerSolid(){
    itsMarkerSolidVolume = new G4Box( itsName+"_marker_solid",
				      itsXLength,
				      itsYLength,
				      itsLength/2);
}

void BDSAcceleratorComponent::BuildBendMarkerSolid(){
  G4double transverseSize=std::max(itsXLength, itsYLength);
  G4double xHalfLengthPlus, xHalfLengthMinus;
  
  xHalfLengthPlus = (itsLength + (transverseSize/2.0)*(tan(itsPhiAngleIn) -tan(itsPhiAngleOut)))/2.0;
  xHalfLengthMinus = (itsLength +  (transverseSize/2.0)*(tan(itsPhiAngleOut)-tan(itsPhiAngleIn )))/2.0;
  
  
  if((xHalfLengthPlus<0) || (xHalfLengthMinus<0)){
    G4cerr << "Bend radius in " << itsName << " too small for this tunnel/component geometry. Exiting." << G4endl;
    G4cerr << "xHalfLengthPlus:  " << xHalfLengthPlus << G4endl;
    G4cerr << "xHalfLengthMinus: " << xHalfLengthMinus << G4endl;  
    exit(1);
  }
  
#ifdef BDSDEBUG  
  G4cout << "BDSAcceleratorComponent::MakeDefaultMarkerLogicalVolume> Trap parameters:  " << G4endl;
  G4cout  <<   
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
#endif 

  itsMarkerSolidVolume = new G4Trap(itsName+"_trapezoid_marker",
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

  /*Is this equivalent to the G4Trap above?
  itsMarkerSolidVolume = new G4Trd(itsName+"_marker",
				   xHalfLengthPlus,     // x hlf lgth at +z
				   xHalfLengthMinus,    // x hlf lgth at -z
				   transverseSize/2,    // y hlf lgth at +z
				   transverseSize/2,    // y hlf lgth at -z
				   fabs(cos(itsAngle/2))*transverseSize/2);// z hlf lgth
  */
}

void BDSAcceleratorComponent::BuildMarkerLogicalVolume(){ //To avoid code repetition a default marker logical volume is provided here
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  CalculateLengths();
  if ((itsPhiAngleIn==0)&&(itsPhiAngleOut==0)){
    BuildStraightMarkerSolid();
  } else {
    BuildBendMarkerSolid();
  }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - making logical volume object..." << G4endl;
#endif
  itsMarkerLogicalVolume=new G4LogicalVolume(
					     itsMarkerSolidVolume,
					     BDSMaterials::Instance()->GetMaterial("vacuum"),
					     itsName+"_log");
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - setting user limits..." << G4endl;
#endif
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsMarkerUserLimits =  new G4UserLimits();
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsMarkerUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
  // now protect the fields inside the marker volume by giving the
  // marker a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - setting field manager..." << G4endl;
#endif
    itsMarkerLogicalVolume->
      SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
#ifdef BDSDEBUG
    G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSAcceleratorComponent::SetTunnel(BDSTunnel* val){
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  itsTunnel = val;
#ifdef BDSDEBUG 
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSAcceleratorComponent::SetMultiplePhysicalVolumes(std::vector<G4VPhysicalVolume*> val){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for(std::vector<G4VPhysicalVolume*>::const_iterator it = val.begin();
      it != val.end(); it++){
    SetMultiplePhysicalVolumes(*it);
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSAcceleratorComponent::BuildTunnel(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
    if(itsTunnel != NULL){
      itsTunnel->Build();
      SetMultiplePhysicalVolumes(itsTunnel->multiplePhysicalVolumes());
    } else {
      std::stringstream ss;
      ss << "BDSAcceleratorComponent::BuildTunnel() - attempting to build tunnel but no tunnel assigned to accelerator component " << itsName ;
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSAcceleratorComponent::PrepareField(G4VPhysicalVolume *referenceVolume)
{
  // creates a field mesh in the reference frame of a physical volume
  // from  b-field map value list 
  // has to be called after the component is placed in the geometry
  if(itsMagField!=NULL) {
    itsMagField->Prepare(referenceVolume);
  } 
}



void BDSAcceleratorComponent::CalculateLengths(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  double tunnelOffsetX=0;
  double tunnelOffsetY=0;
  double tunnelThickness=0;
  double tunnelRadius=0;
  if(this->GetTunnel() != NULL){
    tunnelOffsetX=GetTunnel()->offsetX();
    tunnelOffsetY=GetTunnel()->offsetY();
    tunnelRadius=GetTunnel()->radius();
    tunnelThickness=GetTunnel()->thickness()+GetTunnel()->soilThickness();
  } 

  itsXLength = itsYLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
  itsXLength = std::max(itsXLength, tunnelRadius+2*std::abs(tunnelOffsetX) + tunnelThickness+ 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  itsYLength = std::max(itsYLength, tunnelRadius+2*std::abs(tunnelOffsetY) + tunnelThickness+ 4*BDSGlobalConstants::Instance()->GetLengthSafety() );
  
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
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

void BDSAcceleratorComponent::BuildFieldAndStepper(){
  if(itsBmap!=""){
    G4cout << __METHOD_NAME__ << " - building bmap field and stepper." << G4endl;
    BuildBmapFieldAndStepper();
  } else {
    G4cout << __METHOD_NAME__ << " - building beampipe field and stepper." << G4endl;
    BuildBPFieldAndStepper();
    SetBPFieldMgr();
  }
}

void BDSAcceleratorComponent::BuildBPFieldAndStepper(){;}

void BDSAcceleratorComponent::SetBPFieldMgr(){;}

void BDSAcceleratorComponent::BuildFieldMgr(){
  BuildFieldMgr(itsStepper, itsMagField);
}

void BDSAcceleratorComponent::BuildFieldMgr(G4MagIntegratorStepper* aStepper,
					    G4MagneticField* aField){
  itsChordFinder= new G4ChordFinder(aField,
				    BDSGlobalConstants::Instance()->GetChordStepMinimum(),
				    aStepper);
  
  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsFieldMgr= new G4FieldManager();
  itsFieldMgr->SetDetectorField(aField);
  itsFieldMgr->SetChordFinder(itsChordFinder);
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0){
    itsFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  }
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>0)
    itsFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0)
    itsFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0)
    itsFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());

}


void BDSAcceleratorComponent::BuildBmapFieldAndStepper()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - building magnetic field...." << G4endl;
#endif

  BDSMagFieldFactory* bFact = new BDSMagFieldFactory();
  itsMagField = bFact->buildMagField(itsBmap, itsBmapZOffset, _geom, itsBmapXOffset);
    
  itsEqRhs = new G4Mag_UsualEqRhs(itsMagField);
  itsStepper = new G4NystromRK4(itsEqRhs);

  if(itsStepper && itsMagField){
    BuildFieldMgr(itsStepper, itsMagField);
    itsMarkerLogicalVolume->SetFieldManager(itsFieldMgr,true);
  }

  /*
  // create a field manager
  G4FieldManager* fieldManager = new G4FieldManager();
  fieldManager->SetDetectorField(itsMagField );
  
#ifdef BDSDEBUG
  G4cout << "Setting stepping accuracy parameters..." << G4endl;
#endif
  
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0){
    fieldManager->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
  }
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0){
    fieldManager->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  }
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>=0){
    fieldManager->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  }
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0){
    fieldManager->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  }
  
  G4MagInt_Driver* fIntgrDriver = new G4MagInt_Driver(BDSGlobalConstants::Instance()->GetChordStepMinimum(),
						      itsStepper, 
						      itsStepper->GetNumberOfVariables() );
  itsChordFinder = new G4ChordFinder(fIntgrDriver);
  
  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  
  fieldManager->SetChordFinder( itsChordFinder ); 
  
  G4bool forceToAllDaughters=true;
  
#ifdef BDSDEBUG
  G4cout << "Setting the logical volume " << itsMarkerLogicalVolume->GetName() << " field manager... force to all daughters = " << forceToAllDaughters << G4endl;
#endif
itsMarkerLogicalVolume->SetFieldManager(fieldManager,forceToAllDaughters);
  */
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
