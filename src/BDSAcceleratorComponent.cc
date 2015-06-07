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
						  G4String aTunnelMaterial,
						  G4String aMaterial,
						  G4double angle,
						  G4double XOffset, 
						  G4double YOffset,
						  G4double ZOffset, 
						  G4double tunnelRadius, 
						  G4double tunnelOffsetX,
						  G4String aTunnelCavityMaterial,
						  BDSTiltOffset tiltOffsetIn):
  BDSGeometryComponent(NULL,NULL),
  itsName(aName),
  itsLength(aLength),
  itsBpRadius(aBpRadius),
  itsXAper(aXAper),
  itsYAper(aYAper),
  itsAngle(angle),
  itsMaterial(aMaterial),
  itsTunnelMaterial(aTunnelMaterial),
  itsXOffset(XOffset),
  itsYOffset(YOffset), 
  itsZOffset(ZOffset), 
  itsTunnelRadius(tunnelRadius), 
  itsTunnelOffsetX(tunnelOffsetX),
  itsTunnelCavityMaterial(aTunnelCavityMaterial),
  tiltOffset(tiltOffsetIn)
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
  itsTunnelMaterial(aTunnelMaterial),
  itsXOffset(XOffset),
  itsYOffset(YOffset), 
  itsZOffset(ZOffset), 
  itsTunnelRadius(tunnelRadius), 
  itsTunnelOffsetX(tunnelOffsetX), 
  itsTunnelCavityMaterial(aTunnelCavityMaterial),
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
  if (itsTunnelRadius<=BDSGlobalConstants::Instance()->GetLengthSafety()){
    itsTunnelRadius=BDSGlobalConstants::Instance()->GetTunnelRadius();
  }
  CalculateLengths(); // Calculate dimensions based on component and tunnel dimensions
  itsOuterLogicalVolume=NULL;
  itsMarkerLogicalVolume=NULL;
  itsTunnelLogicalVolume=NULL;
  itsTunnelFloorLogicalVolume=NULL;
  //itsRotation=NULL;
  itsOuterStepper=NULL;
  itsOuterUserLimits=NULL;
  itsMarkerUserLimits=NULL;
  itsInnerBeampipeUserLimits=NULL;
  itsInnerMostLogicalVolume=NULL;
  itsPrecisionRegion=0;
  itsMarkerSolidVolume=NULL;
  itsTunnelSolid=NULL;
  itsSoilSolid=NULL;
  itsInnerTunnelSolid=NULL;
  itsTunnelCavity=NULL;
  itsLargerTunnelCavity=NULL;
  itsTunnelFloor=NULL;
  itsLargerInnerTunnelSolid=NULL;
  itsTunnelMinusCavity=NULL;
  itsTunnelSizedBlock=NULL;
  itsBLMLogicalVolume=NULL;
  itsBlmCaseLogicalVolume=NULL;
  itsSoilTunnelLogicalVolume=NULL;
  itsTunnelCavityLogicalVolume=NULL;
  itsTunnelMinusCavityLogicalVolume=NULL;
  itsTunnelPhysiInner=NULL;
  itsTunnelPhysiComp=NULL;
  itsTunnelFloorPhysiComp=NULL;
  itsTunnelPhysiCompSoil=NULL;
  itsTunnelUserLimits=NULL;
  itsSoilTunnelUserLimits=NULL;
  itsInnerTunnelUserLimits=NULL;

  nullRotationMatrix=NULL;
  tunnelRot=NULL;
  itsVisAttributes=NULL;
  VisAtt=NULL;
  VisAtt1=NULL;
  VisAtt2=NULL;
  itsBLMSolid=NULL;
  itsBlmOuterSolid=NULL;
  itsSPos = 0.0;
  itsCopyNumber = 0;

  readOutLV = NULL;
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
  itsXLength = std::max(itsXLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  itsYLength = std::max(itsYLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );
  
}

/*
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
*/
void BDSAcceleratorComponent::BuildTunnel()
{
  //Make sure length is not zero
  if(itsLength <= BDSGlobalConstants::Instance()->GetLengthSafety()){
    G4cout << "BDSAcceleratorComponent::BuildTunnel() - WARNING - " << G4endl;
    G4cout << "Components length, " << itsLength << ", less than or equal to safety length, " << BDSGlobalConstants::Instance()->GetLengthSafety() << ". Not building tunnel." << G4endl;
    G4Exception("Error: length of component less than safety length", "-1", JustWarning, "");
    return;
  }

  G4String tunnelMaterialName;
  if(itsTunnelMaterial!=""){
    tunnelMaterialName=itsTunnelMaterial;
  } else {
    tunnelMaterialName =BDSGlobalConstants::Instance()->GetTunnelMaterialName();
  }
  G4Material *tunnelMaterial=BDSMaterials::Instance()->GetMaterial(tunnelMaterialName);
  
  G4String soilMaterialName =BDSGlobalConstants::Instance()->GetSoilMaterialName();
  G4Material *soilMaterial=BDSMaterials::Instance()->GetMaterial(soilMaterialName);


#ifdef BDSDEBUG
  G4cout << "Soil :"
         << " r= " << (itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness())/CLHEP::m + BDSGlobalConstants::Instance()->GetTunnelSoilThickness()/CLHEP::m<< " m"
         << " l= " << itsLength/CLHEP::m << " m"
         << " material = " << soilMaterialName << " m"
         << G4endl;
  G4cout << "Outer tunnel :"
         << " r= " << (itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness())/CLHEP::m << " m"
         << " l= " << itsLength/CLHEP::m << " m"
         << " material = " << tunnelMaterialName << " m"
         << G4endl;
  G4cout << "Inner tunnel :"
         << " r= " << itsTunnelRadius/CLHEP::m << " m"
         << " l= " << itsLength/CLHEP::m << " m"
         << G4endl;
#endif

  tunnelRot=new G4RotationMatrix();
  G4ThreeVector tunnelTrans;
  G4ThreeVector floorOffsetThreeVector;
  G4double blockSize = 4*itsTunnelRadius;
  G4ThreeVector nullThreeVector = G4ThreeVector(0,0,0);
  nullRotationMatrix = new G4RotationMatrix();


  //  if ( itsAngle/itsLength < 1.25e-4 ){ //Build a straight tunnel using tubes
  if ( itsAngle==0 ){ //Build a straight tunnel using tubes
  itsTunnelSizedBlock = new G4Box(
                                  itsName+"_bnd_sized_block_solid", 
                                  blockSize,
                                  blockSize,
                                  (itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())
                                  );

   itsTunnelSolid=new G4Tubs(itsName+"_tun_solid",
                             0,
                             itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness(),
                             (itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0,
                             0,CLHEP::twopi*CLHEP::radian);
   
   itsSoilSolid=new G4Tubs(itsName+"_tun_soil_solid",
                           itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety(),
                           itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness(),	
                           (itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0,
                           0,CLHEP::twopi*CLHEP::radian);
   
   itsInnerTunnelSolid=new G4Tubs(itsName+"_inner_tun_solid",
                                  0.,
                                  itsTunnelRadius,
                                  (itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())/2,
                                  0,CLHEP::twopi*CLHEP::radian);
   
   itsLargerInnerTunnelSolid=new G4Tubs(itsName+"_inner_tun_solid",
					0.,
					itsTunnelRadius+BDSGlobalConstants::Instance()->GetLengthSafety(),
					(itsLength-BDSGlobalConstants::Instance()->GetLengthSafety()),
					0,CLHEP::twopi*CLHEP::radian);
   
   tunnelTrans.setX(itsTunnelOffsetX);
   tunnelTrans.setY(BDSGlobalConstants::Instance()->GetTunnelOffsetY());
   tunnelTrans.setZ(0);

   floorOffsetThreeVector = G4ThreeVector(0,-blockSize-BDSGlobalConstants::Instance()->GetTunnelFloorOffset(),0);

   
   itsTunnelCavity = new G4SubtractionSolid(
                                         itsName +"_tun_cavity_solid", 
                                         itsInnerTunnelSolid,
                                         itsTunnelSizedBlock,
                                         nullRotationMatrix,
                                         floorOffsetThreeVector
                                         );

   itsLargerTunnelCavity = new G4SubtractionSolid(
                                         itsName +"_tun_cavity_solid", 
                                         itsLargerInnerTunnelSolid,
                                         itsTunnelSizedBlock,
                                         nullRotationMatrix,
                                         floorOffsetThreeVector
                                         );

  } else {//Build a trapezoidal tunnel to follow the bends

    G4double pi_ov_2 = asin(1.);
    tunnelRot->rotateY(pi_ov_2);

    G4double xHalfLengthPlus, xHalfLengthMinus, tunHalfLen;
    xHalfLengthMinus = (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2)) * (itsTunnelRadius + BDSGlobalConstants::Instance()->GetTunnelThickness()) * tan(itsAngle/2)/2;
    
    xHalfLengthPlus = (itsLength/itsAngle)*sin(itsAngle/2)
      + fabs(cos(itsAngle/2)) * (itsTunnelRadius + BDSGlobalConstants::Instance()->GetTunnelThickness()) * tan(itsAngle/2)/2;
    
    tunHalfLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
    tunnelTrans.setZ(0);
    tunnelTrans.setX(0);
    tunnelTrans.setY(BDSGlobalConstants::Instance()->GetTunnelOffsetY());

    

#ifdef BDSDEBUG
    G4cout << "Building a block" << G4endl;
#endif
    itsTunnelSizedBlock = new G4Box(
                                    itsName+"_bnd_sized_block_solid", 
                                    blockSize,
                                    blockSize,
                                    tunHalfLen
				    );

    
#ifdef BDSDEBUG
    G4cout << "Building a tunnel solid" << G4endl;
#endif

    itsTunnelSolid = new G4IntersectionSolid(
                                             itsName+"_tun_solid",
                                             new G4Tubs(
                                                        itsName+"_temp_tun_solid",
                                                        0,
                                                        itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness(),
                                                        tunHalfLen,
                                                        0,CLHEP::twopi*CLHEP::radian),			    
                                             itsMarkerLogicalVolume->GetSolid(),
					     BDSGlobalConstants::Instance()->RotYM90(),
                                             nullThreeVector
                                             ); 
#ifdef BDSDEBUG
    G4cout << "Building a soil tunnel solid" << G4endl;
#endif
    itsSoilSolid = new G4IntersectionSolid(
                                           itsName+"_soil_solid",
                                           new G4Tubs(
                                                      itsName+"_temp_soil_solid",
                                                      itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety(),
                                                      itsTunnelRadius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness(),
                                                      tunHalfLen,
                                                      0,CLHEP::twopi*CLHEP::radian),
                                           itsMarkerLogicalVolume->GetSolid(),
                                           BDSGlobalConstants::Instance()->RotYM90(),
                                           nullThreeVector
                                           ); 
#ifdef BDSDEBUG   
    G4cout << "Building inner tunnel solid" << G4endl;
#endif
    itsInnerTunnelSolid=new G4IntersectionSolid(
                                                itsName+"_inner_tun_solid",
                                                new G4Tubs(itsName+"_temp_inner_tun_solid",
                                                           0.,
                                                           itsTunnelRadius,
                                                           tunHalfLen,
                                                           0,CLHEP::twopi*CLHEP::radian),
                                                itsMarkerLogicalVolume->GetSolid(),
                                                BDSGlobalConstants::Instance()->RotYM90(),
                                                nullThreeVector
                                                ); 
#ifdef BDSDEBUG
    G4cout << "Building larger inner tunnel solid" << G4endl;
#endif
    itsLargerInnerTunnelSolid= new G4Tubs(itsName+"_temp_inner_tun_solid",
                                          0.,
                                          itsTunnelRadius+BDSGlobalConstants::Instance()->GetLengthSafety(),
                                          2*tunHalfLen,
                                          0,CLHEP::twopi*CLHEP::radian);
                                                       
    
    floorOffsetThreeVector = G4ThreeVector(0,-blockSize-BDSGlobalConstants::Instance()->GetTunnelFloorOffset(),0);
    
#ifdef BDSDEBUG
    G4cout << "Building tunnel cavity" << G4endl;
#endif
    itsTunnelCavity = new G4SubtractionSolid(
                                         itsName +"_tun_cavity_solid", 
                                         itsInnerTunnelSolid,
                                         itsTunnelSizedBlock,
                                         nullRotationMatrix,
                                         floorOffsetThreeVector
                                         );
#ifdef BDSDEBUG
    G4cout << "Building larger tunnel cavity" << G4endl;
#endif
   itsLargerTunnelCavity = new G4SubtractionSolid(
                                         itsName +"_tun_cavity_solid", 
                                         itsLargerInnerTunnelSolid,
                                         itsTunnelSizedBlock,
                                         nullRotationMatrix,
                                         floorOffsetThreeVector
                                         );
  }   //End of "else" statement relevant to bending tunnel

  
#ifdef BDSDEBUG
  G4cout << "Building tunnel minus cavity" << G4endl;
#endif
  itsTunnelMinusCavity = new G4SubtractionSolid(
                                                itsName+"_tun_floor_and_tun",
                                                itsTunnelSolid,
                                                itsLargerTunnelCavity
                                                );
#ifdef BDSDEBUG
  G4cout << "Building tunnel minus cavity logical volume" << G4endl;
#endif

  itsTunnelMinusCavityLogicalVolume=	
    new G4LogicalVolume(itsTunnelMinusCavity,
                        tunnelMaterial,
                        itsName+"_tun_flr_and_tun_log");
  
#ifdef BDSDEBUG
  G4cout << "Building soil tunnel logical volume" << G4endl;
#endif
  itsSoilTunnelLogicalVolume=	
    new G4LogicalVolume(itsSoilSolid,
                        soilMaterial,
                        itsName+"_tun_soil_log");
#ifdef BDSDEBUG
  G4cout << "Building tunnel logical volume" << G4endl;
#endif
  itsTunnelLogicalVolume=	
    new G4LogicalVolume(itsTunnelSolid,
                        tunnelMaterial,
                        itsName+"_tun_log");
#ifdef BDSDEBUG
  G4cout << "Building tunnel cavity logical volume" << G4endl;
#endif

  G4String tunnelCavityMaterialName;
  if(itsTunnelCavityMaterial!=""){
    tunnelCavityMaterialName=itsTunnelCavityMaterial;
  } else {
    tunnelCavityMaterialName =BDSGlobalConstants::Instance()->GetTunnelCavityMaterialName();
  }

  itsTunnelCavityLogicalVolume=	
    new G4LogicalVolume(itsLargerTunnelCavity,
                        BDSMaterials::Instance()->GetMaterial(tunnelCavityMaterialName),
                        itsName+"_tun_cav_log");
#ifdef BDSDEBUG
  G4cout << "Placing tunnel minus cavity" << G4endl;
#endif
  itsTunnelPhysiComp = new G4PVPlacement(
                                         tunnelRot,		     // no rotation
                                         tunnelTrans,	                     // at (0,0,0)
                                         itsTunnelMinusCavityLogicalVolume,  // its logical volume
                                         itsName+"_tun_phys",	     // its name
                                         itsMarkerLogicalVolume,    // its mother  volume
                                         true,		     // no boolean operation
                                         0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number
  
  
  SetMultiplePhysicalVolumes(itsTunnelPhysiComp);

#ifdef BDSDEBUG
  G4cout << "Placing soil around tunnel" << G4endl;
#endif
  itsTunnelPhysiCompSoil = new G4PVPlacement(
                                             tunnelRot,		     // no rotation
                                             tunnelTrans,	                     // at (0,0,0)
                                             itsSoilTunnelLogicalVolume,  // its logical volume
                                             itsName+"_tun_phys_soil",	     // its name
                                             itsMarkerLogicalVolume,    // its mother  volume
                                             true,		     // no boolean operation
                                             0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number
  
  SetMultiplePhysicalVolumes(itsTunnelPhysiCompSoil);
  /*
    itsTunnelPhysiInner = new G4PVPlacement(
    tunnelRot,
    tunnelTrans,	                // at (0,0,0)
    itsTunnelCavityLogicalVolume,  // its logical volume
    itsName+"_inner_tun_phys",// its name
    itsMarkerLogicalVolume,   // its mother  volume
    false,		        // no boolean operation
    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		        // copy number
  */
  
#ifndef NOUSERLIMITS
  itsTunnelUserLimits = new G4UserLimits("tunnel cuts");
  itsSoilTunnelUserLimits = new G4UserLimits("tunnel soil cuts");
  itsInnerTunnelUserLimits = new G4UserLimits("inner tunnel cuts");
  G4double tcut = BDSGlobalConstants::Instance()->GetThresholdCutCharged();
  if(tcut>0){
    itsTunnelUserLimits->SetUserMinEkine(tcut);
    itsSoilTunnelUserLimits->SetUserMinEkine(tcut);
    itsInnerTunnelUserLimits->SetUserMinEkine(tcut);
  }
  
  itsTunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsSoilTunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsInnerTunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());

  itsTunnelUserLimits->SetMaxAllowedStep(itsLength);
  itsSoilTunnelUserLimits->SetMaxAllowedStep(itsLength);
  itsInnerTunnelUserLimits->SetMaxAllowedStep(itsLength);
  
  itsTunnelMinusCavityLogicalVolume->SetUserLimits(itsTunnelUserLimits);
  itsSoilTunnelLogicalVolume->SetUserLimits(itsSoilTunnelUserLimits);
  itsTunnelCavityLogicalVolume->SetUserLimits(itsInnerTunnelUserLimits);
#endif
  
#ifdef BDSDEBUG
  G4cout << "Setting vis attributes" << G4endl;
#endif
  //
    // set visualization attributes
    //
  VisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  VisAtt->SetVisibility(true);
  
  itsSoilTunnelLogicalVolume->SetVisAttributes(VisAtt);
  VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  VisAtt1->SetVisibility(BDSGlobalConstants::Instance()->GetShowTunnel());
  VisAtt2 = new G4VisAttributes(G4Colour(0.0, 0.5, 0.5));
  VisAtt2->SetVisibility(false);
  itsTunnelMinusCavityLogicalVolume->SetVisAttributes(VisAtt1);
  itsMarkerLogicalVolume->SetVisAttributes(VisAtt);
  itsTunnelCavityLogicalVolume->SetVisAttributes(VisAtt2);
  
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
  itsGateLogicalVolume->SetVisAttributes(VisAtt);
  }
*/
//  LocalWords:  itsTunnelUserLimits
