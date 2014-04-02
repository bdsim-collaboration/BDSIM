/* BDSIM code.    
A scintillator screen.
Work in progress.  
*/

#include "BDSGlobalConstants.hh" 
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSSampler.hh"
#include "BDSSamplerSD.hh"
#include "BDSOutput.hh"
#include "BDSCCDCamera.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "BDSDebug.hh"

#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "parser/gmad.h"
#include <map>
#include "BDSAwakeMultilayerScreen.hh"
//#include "UltraFresnelLens.hh"
//#include "UltraFresnelLensParameterisation.hh"

extern BDSSamplerSD* BDSSamplerSensDet;
extern BDSOutput* bdsOutput;

//============================================================
typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSAwakeScintillatorScreen::BDSAwakeScintillatorScreen (G4String aName, G4String material, G4double thickness):
  BDSAcceleratorComponent(aName, 1.0, 0, 0, 0, SetVisAttributes()),_material(material), _thickness(thickness)
{
  //Set the rotation of the screen
  _screenRotationMatrix = new G4RotationMatrix();
  _screenAngle=-45*BDSGlobalConstants::Instance()->GetPI()/180.0;
  _screenRotationMatrix->rotateY(_screenAngle);

  _vacRotationMatrix = new G4RotationMatrix();

  itsType="awakescreen";
  SetName(aName);
  if ( (*LogVolCount)[itsName]==0)
    {
      Build();
      //      SetVisAttributes();
      
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=GetMarkerLogicalVolume();
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }  
}

G4VisAttributes* BDSAwakeScintillatorScreen::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceWireframe(true);

  _visAttFront=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  _visAttScint=new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.5));
  _visAttBase =new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.5));
  _visAttSampler=new G4VisAttributes(G4Colour(0.2,0.2,0.0,0.5));
  

  _visAttFront->SetForceSolid(true);
  _visAttScint->SetForceSolid(true);
  _visAttBase->SetForceSolid(true);
  _visAttSampler->SetForceSolid(true);
  _visAttSampler->SetVisibility(true);



  return itsVisAttributes;
}

void BDSAwakeScintillatorScreen::BuildCameraScoringPlane(){
  G4String tmp = "_cameraScoringPlane";
  _scoringPlaneName=itsName+tmp;
  int nThisSampler= BDSSampler::GetNSamplers() + 1;
  G4String ident="_camera";
  _samplerName = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName);
  _samplerName2 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName+"_2");

  
  //Build and place the volume...
  itsCameraScoringPlaneSolid = new G4Box("CameraScoringPlaneSolid",100*CLHEP::mm/2.0,500*CLHEP::mm/2.0,_scoringPlaneThickness/2.0);

  itsCameraScoringPlaneLog = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog",0,0,0);
  itsCameraScoringPlaneLog->SetVisAttributes(_visAttSampler);

  G4double dispX=_cameraScreenDist-_scoringPlaneThickness/2.0;
  G4double dispY=0;
  G4double dispZ=-_cameraScreenDist/2.0;;

  new G4PVPlacement(BDSGlobalConstants::Instance()->RotY90(),G4ThreeVector(dispX,dispY,dispZ),itsCameraScoringPlaneLog,_samplerName,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  itsCameraScoringPlaneLog2 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog2",0,0,0);
  itsCameraScoringPlaneLog2->SetVisAttributes(_visAttSampler);

  G4double dispX2=-sin(_screenAngle)*_cameraScreenDist;
  G4double dispY2=0;
  G4double dispZ2=cos(_screenAngle)*_cameraScreenDist-_cameraScreenDist/2.0;


  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(dispX2,dispY2,dispZ2),itsCameraScoringPlaneLog2,_samplerName2,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  
  (*LogVol)[_samplerName]=itsCameraScoringPlaneLog;
  (*LogVol)[_samplerName2]=itsCameraScoringPlaneLog2;
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  if(BDSSampler::GetNSamplers()==0){
    BDSSamplerSensDet = new BDSSamplerSD(itsName, "plane");
    SDMan->AddNewDetector(BDSSamplerSensDet);
  }
  itsCameraScoringPlaneLog->SetSensitiveDetector(BDSSamplerSensDet);
  //SPM bdsOutput->nSamplers++;
  BDSSampler::AddExternalSampler();
  bdsOutput->SampName.push_back(_samplerName+"_1");
  BDSSampler::AddExternalSampler();
  bdsOutput->SampName.push_back(_samplerName2+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsCameraScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}

//void BDSAwakeScintillatorScreen::BuildFresnelLens(){
  ////////////////////////////////////////////////////////////////////////////////////////////////////////                                                    
  /*
  G4cout << "#                                                    #" << G4endl ;
  G4cout << "#           Building the Fresnel lens ...            #" << G4endl ;
  G4cout << "#                                                    #" << G4endl ;

  G4double      LensDiameter        = 457*CLHEP::mm ; // Size of the optical active area of the lens.                                                                
  G4int      LensNumOfGrooves    = 13 ;
  //G4int      LensNumOfGrooves    = 129 ;                                                                                                                    
  //G4int      LensNumOfGrooves    = 1287 ;                                                                                                                   

  G4double      LensBorderThickness = 2.8*CLHEP::mm ;     // Thickness of the border area.                                                                           
  G4double      LensFocalLength     = 441.973*CLHEP::mm ; // This parameter depends on the lens geometry, etc !!                                                     
  G4Material   *LensMaterial        = G4Material::GetMaterial(name = "Acrylic") ;
  G4ThreeVector LensPosition        = UVscopePosition+G4ThreeVector(0.0*CLHEP::mm,0.0*CLHEP::mm,UVscopeHeight/2.0-UVscopeBaffle) ;

  UltraFresnelLens *FresnelLens = new UltraFresnelLens(LensDiameter,LensNumOfGrooves,LensMaterial,_log) ;
  */
//}


void BDSAwakeScintillatorScreen::BuildScreenScoringPlane(){
  G4String tmp = "_screenScoringPlane";
  _screenScoringPlaneName=itsName+tmp;
  int nThisSampler= BDSSampler::GetNSamplers() + 1;
  G4String ident="_screen";
  _screenSamplerName = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_screenScoringPlaneName);
  _screenSamplerName2 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_screenScoringPlaneName+"_2");
  
  //Build and place the volume...
  itsScreenScoringPlaneSolid = new G4Box("ScreenScoringPlaneSolid",_screenWidth/2.0,_screenHeight/2.0,_scoringPlaneThickness/2.0);
  itsScreenScoringPlaneLog = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"ScreenScoringPlaneLog",0,0,0);
  itsScreenScoringPlaneLog->SetVisAttributes(_visAttSampler);
  itsScreenScoringPlaneLog2 = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"ScreenScoringPlaneLog2",0,0,0);
  itsScreenScoringPlaneLog2->SetVisAttributes(_visAttSampler);
  G4double dispX=0;
  G4double dispY=0;
  G4double dispZ=2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;
  G4double dispZ2=-2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;
  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ),itsScreenScoringPlaneLog,_screenSamplerName,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());

  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ2),itsScreenScoringPlaneLog2,_screenSamplerName2,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  //---Removing downstream sampler
  //---  (*LogVol)[_screenSamplerName]=itsScreenScoringPlaneLog;
  //---
  (*LogVol)[_screenSamplerName2]=itsScreenScoringPlaneLog2;
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  if(BDSSampler::GetNSamplers()==0){
    BDSSamplerSensDet = new BDSSamplerSD(itsName, "plane");
    SDMan->AddNewDetector(BDSSamplerSensDet);
  }
  //---Removing downstream sampler
  //---  itsScreenScoringPlaneLog->SetSensitiveDetector(BDSSamplerSensDet);
  //---
  itsScreenScoringPlaneLog2->SetSensitiveDetector(BDSSamplerSensDet);
  //SPM bdsOutput->nSamplers++;
  //---Removing downstream sampler
  //---BDSSampler::AddExternalSampler();
  //---bdsOutput->SampName.push_back(_screenSamplerName+"_1");
  //---
  BDSSampler::AddExternalSampler();
  bdsOutput->SampName.push_back(_screenSamplerName2+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsScreenScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}

void BDSAwakeScintillatorScreen::Build(){
      BuildScreen();
      BuildCamera();	
      ComputeDimensions();
      BuildMarkerVolume();
      BuildVacuumChamber1();
      BuildScreenScoringPlane();
      BuildCameraScoringPlane();
      PlaceScreen();
      //      PlaceCamera();
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
	BuildTunnel();
      }
}

void BDSAwakeScintillatorScreen::BuildCamera(){
  _camera=new BDSCCDCamera();
}
void BDSAwakeScintillatorScreen::PlaceCamera(){
  _camera->phys(new G4PVPlacement(_screenRotationMatrix,
				  G4ThreeVector(-1*_cameraScreenDist*sin(_screenAngle),0,1*_cameraScreenDist*cos(_screenAngle)),
				  _camera->log(),
				  _camera->name()+"_phys",
				  itsMarkerLogicalVolume,
				  false,
				  0,
				  true)
		);
}

void BDSAwakeScintillatorScreen::BuildScreen()
{
  G4cout << "Building BDSAwakeMultilayerScreen...." << G4endl;
  _mlScreen = new BDSAwakeMultilayerScreen(_material,_thickness);
  
  G4cout << "finished." << G4endl;
  //  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
  //    for(int i=0; i<_mlScreen->nLayers(); i++){
  //      SetSensitiveVolume(_mlScreen[i].log());
  //    }
  //  } 
  G4cout << "BDSAwakeScintillatorScreen: finished building geometry" << G4endl;
}

void BDSAwakeScintillatorScreen::PlaceScreen(){
  _mlScreen->place(_screenRotationMatrix,
		   G4ThreeVector(0,0,-_cameraScreenDist/2.0),
		   itsMarkerLogicalVolume
		   );
}

void BDSAwakeScintillatorScreen::ComputeDimensions(){
  //  itsXLength = itsYLength = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  //  itsXLength = std::max(itsXLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  //  itsYLength = std::max(itsYLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  _cameraScreenDist=(2.0)*CLHEP::m;

  _screenWidth=_mlScreen->size().x();
  _screenHeight=_mlScreen->size().y();

  //The scoring plane...
  _scoringPlaneThickness=1*CLHEP::um;

  _screenThickness = _mlScreen->size().z();
  
  _totalThickness =  
    _screenThickness + 2*_scoringPlaneThickness;
  
  
  //  G4double thi=_totalThickness+2*_cameraScreenDist+2*_camera->size().z()+2*_scoringPlaneThickness;

  //Compute the marker volume length according to the screen thickness and width.
  G4double z_wid = _screenWidth * std::sin(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double z_thi = _totalThickness * std::cos(std::abs(_screenAngle));//Length due to the screen thickness
  G4double x_wid = _screenWidth * std::cos(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double x_thi = _totalThickness * std::sin(std::abs(_screenAngle));//Length due to the screen thickness
  
  //Vacuum chamber dimensions.
  _vacKevlarThickness=0.3*mm;
  _vacMylarThickness=0.05*mm;
  _vacThickness=2*mm;
  _vacInnerWidth=4*cm;
  _vacInnerHeight=4*cm;
  _vacWidth=4*cm+_vacKevlarThickness+_vacMylarThickness+_vacThickness;
  _vacHeight=4*cm+2*_vacThickness;
  _vacDispX=-((sqrt(2.0)/2.0)/2.0)*m-_vacWidth/2.0-0.15*mm;

  itsLength  = z_wid + z_thi +_cameraScreenDist;  
  itsXLength = (x_wid +x_thi+2*_vacWidth)+2*_cameraScreenDist;
  itsYLength = std::max(_screenHeight,_camera->size().y());
  itsYLength = std::max(itsYLength,50*CLHEP::cm);
  std::cout << __METHOD_NAME__ << " " << itsLength << " " << itsXLength << " " << itsYLength << std::endl;


  _vacLength=itsLength;

  



}

void BDSAwakeScintillatorScreen::BuildMarkerVolume(){
  itsMarkerSolidVolume=new G4Box( itsName+"_marker_solid",
				  itsXLength/2.0,
				  itsYLength/2.0,
				  itsLength/2.0); //z half length 

  itsMarkerLogicalVolume=new G4LogicalVolume
    (itsMarkerSolidVolume, 
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     itsName+"_marker_log");
  G4VisAttributes* visAtt = new G4VisAttributes(G4Color(0,1,0));
  visAtt->SetForceWireframe(true);
  itsMarkerLogicalVolume->SetVisAttributes(visAtt);
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsMarkerUserLimits =  new G4UserLimits();
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
}

void BDSAwakeScintillatorScreen::BuildVacuumChamber1(){


  G4VSolid* vacuumOuterSolid = new G4Box("vacuumSolid",_vacWidth/2.0,
					 _vacHeight/2.0, 
					 _vacLength/2.0);


  G4VSolid* vacuumSolid = new G4Box("vacuumSolid",_vacInnerWidth/2.0, _vacInnerHeight/2.0, _vacLength/2.0);

  G4LogicalVolume* vacuumOuterLog = new G4LogicalVolume(vacuumOuterSolid, BDSMaterials::Instance()->GetMaterial("iron"), "vacuumOuterLog",0,0,0);

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX,0,0), 
		    vacuumOuterLog, 
		    "awakeScreenOuterVacuumPV", 
		    itsMarkerLogicalVolume, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  G4LogicalVolume* vacuumLog = new G4LogicalVolume(vacuumSolid, BDSMaterials::Instance()->GetMaterial("vacuum"), "vacuumLog",0,0,0);

 
  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector(_vacThickness/2.0-(_vacMylarThickness+_vacKevlarThickness)/2.0,0,0), 
		    vacuumLog, 
		    "awakeScreenVacuumPV", 
		    vacuumOuterLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );
 

  G4VSolid* vacuumWindowSolid = new G4Box("vacuumWindowSolid",(_vacMylarThickness+_vacKevlarThickness)/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* vacuumWindowLog = new G4LogicalVolume(vacuumWindowSolid, BDSMaterials::Instance()->GetMaterial("vacuum"), "vacuumWindowLog",0,0,0);

  G4VSolid* kevlarWindowSolid = new G4Box("kevlarWindowSolid",_vacKevlarThickness/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* kevlarWindowLog = new G4LogicalVolume(kevlarWindowSolid, BDSMaterials::Instance()->GetMaterial("G4_KEVLAR"), "kevlarWindowLog",0,0,0);

  G4VSolid* mylarWindowSolid = new G4Box("mylarWindowSolid",_vacMylarThickness/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* mylarWindowLog = new G4LogicalVolume(mylarWindowSolid, BDSMaterials::Instance()->GetMaterial("G4_MYLAR"), "mylarWindowLog",0,0,0);

  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector(_vacWidth/2.0-(_vacMylarThickness+_vacKevlarThickness)/2.0,0,0), 
		    vacuumWindowLog, 
		    "awakeScreenVacuumWindowPV", 
		    vacuumOuterLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );
  
  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector((_vacKevlarThickness+_vacMylarThickness)/2.0 - _vacKevlarThickness/2.0,0,0), 
		    kevlarWindowLog, 
		    "awakeScreenKevlarVacuumWindowPV", 
		    vacuumWindowLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector(-(_vacKevlarThickness+_vacMylarThickness)/2.0 + _vacMylarThickness/2.0,0,0), 
		    mylarWindowLog, 
		    "awakeScreenMylarVacuumWindowPV", 
		    vacuumWindowLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  G4VisAttributes* vacVisAttributes=new G4VisAttributes(G4Colour(0.3,0.0,0.4,0.5));
  vacVisAttributes->SetForceWireframe(true);
  vacVisAttributes->SetVisibility(true);
  vacuumOuterLog->SetVisAttributes(vacVisAttributes);

  G4VisAttributes* winVisAttributes=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  winVisAttributes->SetForceSolid(true);
  winVisAttributes->SetVisibility(true);
  vacuumWindowLog->SetVisAttributes(winVisAttributes);



}

void BDSAwakeScintillatorScreen::BuildVacuumChamber2(){

}


BDSAwakeScintillatorScreen::~BDSAwakeScintillatorScreen()
{
  delete itsVisAttributes;
}
