/* BDSIM code.    
A scintillator screen.
Work in progress.  
*/

#include "BDSGlobalConstants.hh" 
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSSampler.hh"
#include "BDSSamplerSD.hh"
#include "BDSOutput.hh"
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

extern BDSSamplerSD* BDSSamplerSensDet;
extern BDSOutput* bdsOutput;

//============================================================
typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSAwakeScintillatorScreen::BDSAwakeScintillatorScreen (G4String aName):
  BDSAcceleratorComponent(aName, 1.0, 0, 0, 0, SetVisAttributes())
{
  //Set the rotation of the screen
  _screenRotationMatrix = new G4RotationMatrix();
  _screenAngle=-45*BDSGlobalConstants::Instance()->GetPI()/180.0;
  _screenRotationMatrix->rotateY(_screenAngle);

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
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetForceSolid(true);

  _visAttFront=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  _visAttScint=new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.5));
  _visAttBase =new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.5));
  _visAttSampler=new G4VisAttributes(G4Colour(0.2,0.2,0.0,0.5));
  

  _visAttFront->SetForceSolid(true);
  _visAttScint->SetForceSolid(true);
  _visAttBase->SetForceSolid(true);
  _visAttSampler->SetForceSolid(true);

  return itsVisAttributes;
}

void BDSAwakeScintillatorScreen::BuildCameraScoringPlane(){
  G4String tmp = "_cameraScoringPlane";
  _scoringPlaneName=itsName+tmp;
  int nThisSampler= BDSSampler::GetNSamplers() + 1;
  G4String ident="_camera";
  _samplerName = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName);
  
  //Build and place the volume...
  itsCameraScoringPlaneSolid = new G4Box("CameraScoringPlaneSolid",itsLength/2.0,_yLength/2.0,_scoringPlaneThickness/2.0);
  itsCameraScoringPlaneLog = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog",0,0,0);
  itsCameraScoringPlaneLog->SetVisAttributes(_visAttScint);
  G4double dispX=-_xLength/2.0+_scoringPlaneThickness/2.0;
  G4double dispY=0;
  G4double dispZ=0;
  new G4PVPlacement(BDSGlobalConstants::Instance()->RotY90(),G4ThreeVector(dispX,dispY,dispZ),itsCameraScoringPlaneLog,_samplerName,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  (*LogVol)[_samplerName]=itsCameraScoringPlaneLog;
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  if(BDSSampler::GetNSamplers()==0){
    BDSSamplerSensDet = new BDSSamplerSD(itsName, "plane");
    SDMan->AddNewDetector(BDSSamplerSensDet);
  }
  itsCameraScoringPlaneLog->SetSensitiveDetector(BDSSamplerSensDet);
  //SPM bdsOutput->nSamplers++;
  BDSSampler::AddExternalSampler();
  bdsOutput->SampName.push_back(_samplerName+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsCameraScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}


void BDSAwakeScintillatorScreen::BuildScreenScoringPlane(){
  G4String tmp = "_screenScoringPlane";
  _screenScoringPlaneName=itsName+tmp;
  int nThisSampler= BDSSampler::GetNSamplers() + 1;
  G4String ident="_camera";
  _screenSamplerName = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_screenScoringPlaneName);
  
  //Build and place the volume...
  itsScreenScoringPlaneSolid = new G4Box("ScreenScoringPlaneSolid",_screenWidth/2.0,_screenHeight/2.0,_scoringPlaneThickness/2.0);
  itsScreenScoringPlaneLog = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"ScreenScoringPlaneLog",0,0,0);
  itsScreenScoringPlaneLog->SetVisAttributes(_visAttSampler);
  G4double dispX=0;
  G4double dispY=0;
  G4double dispZ=sqrt(2)*(-_screenThickness/2.0- _scoringPlaneThickness/2.0);
  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ),itsScreenScoringPlaneLog,_screenSamplerName,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  (*LogVol)[_screenSamplerName]=itsScreenScoringPlaneLog;
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  if(BDSSampler::GetNSamplers()==0){
    BDSSamplerSensDet = new BDSSamplerSD(itsName, "plane");
    SDMan->AddNewDetector(BDSSamplerSensDet);
  }
  itsScreenScoringPlaneLog->SetSensitiveDetector(BDSSamplerSensDet);
  //SPM bdsOutput->nSamplers++;
  BDSSampler::AddExternalSampler();
  bdsOutput->SampName.push_back(_screenSamplerName+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsScreenScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}

void BDSAwakeScintillatorScreen::Build(){
      BuildScreen();
      ComputeDimensions();
      BuildMarkerVolume();
      BuildScreenScoringPlane();
      BuildCameraScoringPlane();
      PlaceScreen();
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
	BuildTunnel();
      }
}

void BDSAwakeScintillatorScreen::BuildScreen()
{
  G4cout << "Building BDSAwakeMultilayerScreen...." << G4endl;
  _mlScreen = new BDSAwakeMultilayerScreen();
  G4cout << "finished." << G4endl;
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    for(int i=0; i<_mlScreen->nLayers(); i++){
      SetSensitiveVolume(_mlScreen[i].log());
    }
  } 
  G4cout << "BDSAwakeScintillatorScreen: finished building geometry" << G4endl;
}

void BDSAwakeScintillatorScreen::PlaceScreen(){
  new G4PVPlacement(
		    _screenRotationMatrix,
		    G4ThreeVector(0,0,0),
		    _mlScreen->log(),
		    "multilayerScreen",
		    itsMarkerLogicalVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );                 
}

void BDSAwakeScintillatorScreen::ComputeDimensions(){
  //  _xLength = _yLength = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  //  _xLength = std::max(_xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  //  _yLength = std::max(_yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  _screenWidth=_mlScreen->size().x();
  _screenHeight=_mlScreen->size().y();

  //The scoring plane...
  _scoringPlaneThickness=1*um;

  _screenThickness = _mlScreen->size().z();
  
  _totalThickness =  
    _screenThickness + _scoringPlaneThickness;
  
  //Compute the marker volume length according to the screen thickness and width.
  G4double z_wid = _screenWidth * std::sin(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double z_thi = _totalThickness * std::cos(_screenAngle);//Length due to the screen thickness
  G4double x_wid = _screenWidth * std::cos(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double x_thi = _totalThickness * std::sin(_screenAngle);//Length due to the screen thickness
  itsLength = (z_wid + z_thi);
  _xLength = x_wid +x_thi + 2*_scoringPlaneThickness;
  _yLength = _screenHeight;
}

void BDSAwakeScintillatorScreen::BuildMarkerVolume(){
  itsMarkerSolidVolume=new G4Box( itsName+"_marker_log",
				  _xLength,
				  _yLength,
				  itsLength/2.0); //z half length 

  itsMarkerLogicalVolume=new G4LogicalVolume
    (itsMarkerSolidVolume, 
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


BDSAwakeScintillatorScreen::~BDSAwakeScintillatorScreen()
{
  delete itsVisAttributes;
}
