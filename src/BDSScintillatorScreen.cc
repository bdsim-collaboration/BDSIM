/* BDSIM code.    
A scintillator screen.
Work in progress.  
*/

#include "BDSGlobalConstants.hh" 
#include "BDSScintillatorScreen.hh"
#include "BDSSampler.hh"
#include "BDSSamplerSD.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SDManager.hh"
#include <map>

extern BDSSamplerSD* BDSSamplerSensDet;

//============================================================

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSScintillatorScreen::BDSScintillatorScreen (G4String aName, G4double tScint, G4double angle, G4String scintMaterial, G4String airMaterial):
  BDSAcceleratorComponent(aName, tScint, 0, 0, 0, SetVisAttributes()),_scintillatorLayerMaterial(BDSMaterials::Instance()->GetMaterial(scintMaterial.data())),_airMaterial(BDSMaterials::Instance()->GetMaterial(airMaterial.data())),_screenAngle(angle),_scintillatorThickness(tScint)
{
  //Set the rotation of the screen
  _screenRotationMatrix = new G4RotationMatrix();
  _screenAngle=angle;
  _screenRotationMatrix->rotateY(_screenAngle);
}

void BDSScintillatorScreen::Build()
{
  ComputeDimensions();
  BuildMarkerLogicalVolume();
  if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
    BuildTunnel();
  }
  BuildScintillatorScreen();
}

G4VisAttributes* BDSScintillatorScreen::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
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

void BDSScintillatorScreen::BuildFrontLayer(){
  //The cellulose protective layerx
  itsFrontLayerSolid  = new G4Box("CelluloseFront",_screenWidth/2.0,_screenHeight/2.0,_frontThickness/2.0);
  itsFrontLayerLog = new G4LogicalVolume(itsFrontLayerSolid,BDSMaterials::Instance()->GetMaterial("Cellulose"),"CelluloseFront",0,0,0);
  itsFrontLayerLog->SetVisAttributes(_visAttFront);
  G4double dispZ=_frontThickness/2.0-_screenThickness/2.0;
  itsFrontLayerPhys = new G4PVPlacement(
					_screenRotationMatrix,
					G4ThreeVector(0,0,dispZ),
					itsFrontLayerLog,
					"ScreenCelluloseFrontPhys",
					itsMarkerLogicalVolume,
					false,
					0,
					BDSGlobalConstants::Instance()->GetCheckOverlaps()
					);                 
  SetMultiplePhysicalVolumes(itsFrontLayerPhys);
}

void BDSScintillatorScreen::BuildCameraScoringPlane(){
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
  BDSSampler::AddExternalSampler(_samplerName+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsCameraScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}


void BDSScintillatorScreen::BuildScreenScoringPlane(){
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
  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(dispX,dispY,dispZ),itsScreenScoringPlaneLog,_screenSamplerName,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  (*LogVol)[_screenSamplerName]=itsScreenScoringPlaneLog;
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  if(BDSSampler::GetNSamplers()==0){
    BDSSamplerSensDet = new BDSSamplerSD(itsName, "plane");
    SDMan->AddNewDetector(BDSSamplerSensDet);
  }
  itsScreenScoringPlaneLog->SetSensitiveDetector(BDSSamplerSensDet);
  //SPM bdsOutput->nSamplers++;
  BDSSampler::AddExternalSampler(_screenSamplerName+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsScreenScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}

void BDSScintillatorScreen::BuildScintillatorLayer(){
// The phosphor layer
//   
  G4String name = "ScintillatorLayerPhys";
  G4double dispZ=0;
  itsScintillatorLayerSolid = new G4Box("ScintillatorLayerSolid",_screenWidth/2.0,_screenHeight/2.0,_scintillatorThickness/2.0);
  itsScintillatorLayerLog = new G4LogicalVolume(itsScintillatorLayerSolid,_scintillatorLayerMaterial,"PhosphorLayer",0,0,0);

  itsScintillatorLayerLog->SetVisAttributes(_visAttScint);



  int nThisSampler= BDSSampler::GetNSamplers() + 1;
  _screenSamplerName = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+name);
  
  //Build and place the volume...
  itsScintillatorLayerPhys=  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ),itsScintillatorLayerLog,_screenSamplerName.c_str(),
					       itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsScintillatorLayerPhys);

  /*
    (*LogVol)[_screenSamplerName]=itsScintillatorLayerLog;
    G4SDManager* SDMan = G4SDManager::GetSDMpointer();
    if(BDSSampler::GetNSamplers()==0){
      BDSSamplerSensDet = new BDSSamplerSD(itsName, "plane");
    SDMan->AddNewDetector(BDSSamplerSensDet);
  }
  itsScintillatorLayerLog->SetSensitiveDetector(BDSSamplerSensDet);
  //SPM bdsOutput->nSamplers++;
  BDSSampler::AddExternalSampler();
  bdsOutput->SampName.push_back(_screenSamplerName+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScintillatorLayerUserLimits =  new G4UserLimits();
  itsScintillatorLayerUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsScintillatorLayerLog->SetUserLimits(itsScintillatorLayerUserLimits);
#endif
  */
}

void BDSScintillatorScreen::BuildBaseLayer(){
  //The PET backing layer
  G4double dispZ=_scintillatorThickness+_frontThickness+_baseThickness/2.0-_screenThickness/2.0;
  itsBaseLayerSolid = new G4Box("PETLayer",_screenWidth/2.0,_screenHeight/2.0,_baseThickness/2.0);
  itsBaseLayerLog = new G4LogicalVolume(itsBaseLayerSolid,BDSMaterials::Instance()->GetMaterial("PET"),"PETLayer",0,0,0);
  
  itsBaseLayerLog->SetVisAttributes(_visAttBase);
  itsBaseLayerPhys =  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ),itsBaseLayerLog,"ScreenPETLayer",
					itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsBaseLayerPhys);
}

void BDSScintillatorScreen::BuildBackLayer(){
  G4double dispZ=_frontThickness+_scintillatorThickness+_baseThickness+_backThickness/2.0-_screenThickness/2.0;
  itsBackLayerSolid = new G4Box("CelluloseBack",_screenWidth/2.0,_screenHeight/2.0,_backThickness/2.0);
  itsBackLayerLog = new G4LogicalVolume(itsBackLayerSolid,BDSMaterials::Instance()->GetMaterial("Cellulose"),"CelluloseBack",0,0,0);
  itsBackLayerLog->SetVisAttributes(_visAttFront);
  itsBackLayerPhys = new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ),itsBackLayerLog,"ScreenCelluloseBack",
				       itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsBackLayerPhys);
}

void BDSScintillatorScreen::BuildOpticalSurfaces(){
  //Add the optical surfaces.
  //Mirrored surface between the phosphor and the PET backing.
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  /*G4LogicalBorderSurface* Surface = */
  new G4LogicalBorderSurface("phosphor_PET_surface", itsScintillatorLayerPhys, itsBaseLayerPhys, OpSurface);
  G4double sigma_alpha=1.0;
  //TiO2 particles provide the reflectivity in the PET layer hence dielectric-metal
  OpSurface -> SetType(dielectric_metal);
  OpSurface -> SetModel(unified);
  OpSurface -> SetFinish(ground);
  OpSurface -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.8);
  SMPT->AddConstProperty("RINDEX",1.5750);
  OpSurface->SetMaterialPropertiesTable(SMPT);


  //Surface between the phosphor and the cellulose front.
  G4OpticalSurface* OpSurface2=new G4OpticalSurface("OpSurface2");
  /*G4LogicalBorderSurface* Surface2 = */ 
  new G4LogicalBorderSurface("phosphor_cellulose_surface", itsScintillatorLayerPhys, itsFrontLayerPhys, OpSurface2);
  OpSurface2 -> SetType(dielectric_dielectric);
  OpSurface2 -> SetModel(unified);
  OpSurface2 -> SetFinish(ground);
  OpSurface2 -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT2 = new G4MaterialPropertiesTable();
  SMPT2->AddConstProperty("REFLECTIVITY",0.0);
  SMPT2->AddConstProperty("RINDEX",1.532);
  OpSurface2->SetMaterialPropertiesTable(SMPT2);

  //Surface between the cellulose and the experimental hall.
  
  /*
  G4OpticalSurface* OpSurface3=new G4OpticalSurface("OpSurface3");
  G4LogicalBorderSurface* Surface3 = new 
    G4LogicalBorderSurface("cellulose_hall_surface", itsFrontLayerPhys, itsMarkerPhysicalVolume, OpSurface3);
  OpSurface3 -> SetType(dielectric_dielectric);
  OpSurface3 -> SetModel(unified);
  OpSurface3 -> SetFinish(ground);
  OpSurface3 -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT3 = new G4MaterialPropertiesTable();
  SMPT3->AddConstProperty("REFLECTIVITY",0.0);
  SMPT3->AddConstProperty("RINDEX",1.0);
  OpSurface3->SetMaterialPropertiesTable(SMPT3);
  */
}


void BDSScintillatorScreen::BuildScintillatorScreen()
{
  BuildScintillatorMaterial();
  BuildScintillatorLayer();
  BuildScreenScoringPlane();
  BuildCameraScoringPlane();
  
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
      SetSensitiveVolume(itsScintillatorLayerLog);
  } 
  G4cout << "BDSScintillatorScreen: finished building geometry" << G4endl;
}

void BDSScintillatorScreen::BuildScintillatorMaterial(){
  BuildScintillatorCompound();
  BuildScintillatorOpticalProperties();
}

void BDSScintillatorScreen::BuildScintillatorCompound(){
  
  /*
  //Scintillator grains sudpended in a polyurethane elastomer with a specific fill factor
  G4double fill_factor=0.5; //i.e. fraction by volume
  G4double yag_screen_density=fill_factor*BDSMaterials::Instance()->GetMaterial("YAG")->GetDensity()+(1-fill_factor)*BDSMaterials::Instance()->GetMaterial("Polyurethane")->GetDensity();
  G4double yag_fraction_by_mass=fill_factor*BDSMaterials::Instance()->GetMaterial("YAG")->GetDensity()/yag_screen_density;
  G4double pur_fraction_by_mass=1-yag_fraction_by_mass;
  _scintillatorLayerMaterial = new G4Material("ScintillatorMaterial", yag_screen_density, 2);
  _scintillatorLayerMaterial->AddMaterial(BDSMaterials::Instance()->GetMaterial("YAG"), yag_fraction_by_mass);
  _scintillatorLayerMaterial->AddMaterial(BDSMaterials::Instance()->GetMaterial("Polyurethane"), pur_fraction_by_mass);
  */
}

void BDSScintillatorScreen::BuildScintillatorOpticalProperties(){
  //Fill the material properties table of the _scintillatorLayerMaterial

  /*The below is required for mie scattering
  G4double scatteringLength=2.17*CLHEP::um;
  G4double anisotropyFactor=0.800;
  _mptScintillatorMaterial->AddConstProperty("MIEHG",scatteringLength);
  _mptScintillatorMaterial->AddConstProperty("MIEHG_FORWARD",anisotropyFactor); 
  _mptScintillatorMaterial->AddConstProperty("MIEHG_BACKWARD",0.0);
  _mptScintillatorMaterial->AddConstProperty("MIEHG_FORWARD_RATIO",1.0);
  //Absoroption length is irrelevant as the screen should be pretty thin
  //  _mptScintillatorMaterial->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption1,     nEntries)
  //    ->SetSpline(true);
  */
  

}


void BDSScintillatorScreen::ComputeDimensions(){
  
  //  _xLength = _yLength = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  //  _xLength = std::max(_xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  //  _yLength = std::max(_yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  _screenWidth=1*CLHEP::m;
  _screenHeight=3*CLHEP::cm;

  //Length due to the screen thickness
  _frontThickness=0;//13*CLHEP::um;
  _baseThickness=0;//275*CLHEP::um;
  _backThickness=0;//13*CLHEP::um;

  //The scoring plane...
  _scoringPlaneThickness=1*CLHEP::um;
  
  _totalThickness =  
    _frontThickness+
    _scintillatorThickness+
    _baseThickness+
    _backThickness+
    _scoringPlaneThickness;

  _screenThickness =  _totalThickness - _scoringPlaneThickness;

  //Compute the marker volume length according to the screen thickness and width.
  G4double z_wid = _screenWidth * std::sin(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double z_thi = _totalThickness * std::cos(_screenAngle);//Length due to the screen thickness
  G4double x_wid = _screenWidth * std::cos(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double x_thi = _totalThickness * std::sin(_screenAngle);//Length due to the screen thickness
  itsLength = (z_wid + z_thi);
  _xLength = x_wid +x_thi + 2*_scoringPlaneThickness;
  _yLength = _screenHeight;
}

void BDSScintillatorScreen::BuildMarkerLogicalVolume(){
  itsMarkerSolidVolume=new G4Box( itsName+"_marker_log",
				  _xLength,
				  _yLength,
				  itsLength/2.0); //z half length 

  itsMarkerLogicalVolume=new G4LogicalVolume
    (itsMarkerSolidVolume, 
     _airMaterial,
     itsName+"_log");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsMarkerUserLimits =  new G4UserLimits();
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
}


BDSScintillatorScreen::~BDSScintillatorScreen()
{
}
