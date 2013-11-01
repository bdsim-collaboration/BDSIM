/* BDSIM code.    
A scintillator screen.
Work in progress.  
*/

#include "BDSGlobalConstants.hh" 
#include "BDSScintillatorScreen.hh"
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
#include "parser/gmad.h"
#include <map>

extern BDSSamplerSD* BDSSamplerSensDet;
extern BDSOutput* bdsOutput;

//============================================================
typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSScintillatorScreen::BDSScintillatorScreen (G4String aName, G4double tScint, G4double angle):
  BDSAcceleratorComponent(aName, tScint, 0, 0, 0, SetVisAttributes()),_scintillatorThickness(tScint)
{
  //Set the rotation of the screen
  _screenRotationMatrix = new G4RotationMatrix();
  _screenAngle=angle;
  _screenRotationMatrix->rotateY(_screenAngle);

  itsType="screen";
  SetName(aName);
  if ( (*LogVolCount)[itsName]==0)
    {
      ComputeDimensions();
      BuildMarkerVolume();
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
	BuildTunnel();
      }
      BuildScintillatorScreen();
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

G4VisAttributes* BDSScintillatorScreen::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetForceSolid(true);

  _visAttFront=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  _visAttScint=new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.5));
  _visAttBase =new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.5));

  _visAttFront->SetForceSolid(true);
  _visAttScint->SetForceSolid(true);
  _visAttBase->SetForceSolid(true);

  return itsVisAttributes;
}

void BDSScintillatorScreen::BuildFrontLayer(){
  //The cellulose protective layerx
  itsFrontLayerSolid  = new G4Box("CelluloseFront",_screenWidth/2.0,_screenHeight/2.0,_frontThickness/2.0);
  itsFrontLayerLog = new G4LogicalVolume(itsFrontLayerSolid,BDSMaterials::Instance()->GetMaterial("Cellulose"),"CelluloseFront",0,0,0);
  itsFrontLayerLog->SetVisAttributes(_visAttFront);
  G4double dispZ=_frontThickness/2.0-_totalThickness/2.0;
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
  G4String tmp = "_scoringPlane";
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

void BDSScintillatorScreen::BuildScintillatorLayer(){
// The phosphor layer
//   
  G4double dispZ=_frontThickness+(_scintillatorThickness)/2.0-_totalThickness/2.0;
  itsScintillatorLayerSolid = new G4Box("ScintillatorLayerSolid",_screenWidth/2.0,_screenHeight/2.0,_scintillatorThickness/2.0);
  itsScintillatorLayerLog = new G4LogicalVolume(itsScintillatorLayerSolid,_scintillatorLayerMaterial,"PhosphorLayer",0,0,0);

  itsScintillatorLayerLog->SetVisAttributes(_visAttScint);
  itsScintillatorLayerPhys=  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ),itsScintillatorLayerLog,"ScintiilatorLayerPhys",
					       itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsScintillatorLayerPhys);
}

void BDSScintillatorScreen::BuildBaseLayer(){
  //The PET backing layer
  G4double dispZ=_scintillatorThickness+_frontThickness+_baseThickness/2.0-_totalThickness/2.0;
  itsBaseLayerSolid = new G4Box("PETLayer",_screenWidth/2.0,_screenHeight/2.0,_baseThickness/2.0);
  itsBaseLayerLog = new G4LogicalVolume(itsBaseLayerSolid,BDSMaterials::Instance()->GetMaterial("PET"),"PETLayer",0,0,0);
  
  itsBaseLayerLog->SetVisAttributes(_visAttBase);
  itsBaseLayerPhys =  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(0,0,dispZ),itsBaseLayerLog,"ScreenPETLayer",
					itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsBaseLayerPhys);
}

void BDSScintillatorScreen::BuildBackLayer(){
  G4double dispZ=_frontThickness+_scintillatorThickness+_baseThickness+_backThickness/2.0-_totalThickness/2.0;
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
  G4LogicalBorderSurface* Surface = new 
    G4LogicalBorderSurface("phosphor_PET_surface", itsScintillatorLayerPhys, itsBaseLayerPhys, OpSurface);
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
  G4LogicalBorderSurface* Surface2 = new 
    G4LogicalBorderSurface("phosphor_cellulose_surface", itsScintillatorLayerPhys, itsFrontLayerPhys, OpSurface2);
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
  BuildCameraScoringPlane();
  
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    SetSensitiveVolume(itsScintillatorLayerLog);
  } 
}

void BDSScintillatorScreen::BuildScintillatorMaterial(){
  BuildScintillatorCompound();
  BuildScintillatorOpticalProperties();
}

void BDSScintillatorScreen::BuildScintillatorCompound(){
  //Crystal YAG
  _scintillatorLayerMaterial = BDSMaterials::Instance()->GetMaterial("YAG");
  G4double birks = 0.08*mm/MeV; 
  _scintillatorLayerMaterial->GetIonisation()->SetBirksConstant(birks);
  
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

  const G4int nEntries = 9;
  G4double PhotonEnergyScintillatorMaterial[nEntries];
  G4double dNEntries2=(G4double)nEntries;
  G4double energyMin=1.91*eV;
  G4double energyMax=2.76*eV;
  G4double deltaEnergy=(energyMax-energyMin)/(dNEntries2-1.0);
  G4double energy=energyMin;
  for(G4int i=0; i<nEntries; energy += deltaEnergy, i++){
    PhotonEnergyScintillatorMaterial[i]=energy;
  }
  G4double RefractiveIndexScintillatorMaterial[nEntries] = //Approximately correct, but check for different wavelengths
    { 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82,
      1.82, 1.82 };
  G4double scintFastScintillatorMaterial[nEntries] = //Approximately correct
    { 0, 0.25, 2.0, 14.0, 13.0, 7.0, 4.0, 2.0, 0.0 };

  const G4int nEntries2 = 32;


  G4double PhotonEnergy[nEntries2] =
    { 2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV,
      2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,
      2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
      2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
      2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV,
      3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV,
      3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
      3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV };
  //                                                        

 G4double Absorption1[nEntries2] =
   {3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m,
    15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
    45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
    52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
    30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
    17.500*m, 14.500*m };

 const int nEntries3=60;
  
 G4double ENERGY_3[nEntries3] = {
   1.56962*eV, 1.58974*eV, 1.61039*eV, 1.63157*eV,
   1.65333*eV, 1.67567*eV, 1.69863*eV, 1.72222*eV,
   1.74647*eV, 1.77142*eV, 1.7971 *eV, 1.82352*eV,
   1.85074*eV, 1.87878*eV, 1.90769*eV, 1.93749*eV,
   1.96825*eV, 1.99999*eV, 2.03278*eV, 2.06666*eV,
   2.10169*eV, 2.13793*eV, 2.17543*eV, 2.21428*eV,
   2.25454*eV, 2.29629*eV, 2.33962*eV, 2.38461*eV,
   2.43137*eV, 2.47999*eV, 2.53061*eV, 2.58333*eV,
   2.63829*eV, 2.69565*eV, 2.75555*eV, 2.81817*eV,
   2.88371*eV, 2.95237*eV, 3.02438*eV, 3.09999*eV,
   3.17948*eV, 3.26315*eV, 3.35134*eV, 3.44444*eV,
   3.54285*eV, 3.64705*eV, 3.75757*eV, 3.87499*eV,
   3.99999*eV, 4.13332*eV, 4.27585*eV, 4.42856*eV,
   4.59258*eV, 4.76922*eV, 4.95999*eV, 5.16665*eV,
   5.39129*eV, 5.63635*eV, 5.90475*eV, 6.19998*eV
 };

 //assume 100 times larger than the rayleigh scattering for now.                                                                                                                                    
 G4double Rayleigh[nEntries3] = {
   167024.4*cm, 158726.7*cm, 150742  *cm,
   143062.5*cm, 135680.2*cm, 128587.4*cm,
   121776.3*cm, 115239.5*cm, 108969.5*cm,
   102958.8*cm, 97200.35*cm, 91686.86*cm,
   86411.33*cm, 81366.79*cm, 76546.42*cm,
   71943.46*cm, 67551.29*cm, 63363.36*cm,
   59373.25*cm, 55574.61*cm, 51961.24*cm,
   48527.00*cm, 45265.87*cm, 42171.94*cm,
   39239.39*cm, 36462.50*cm, 33835.68*cm,
   31353.41*cm, 29010.30*cm, 26801.03*cm,
   24720.42*cm, 22763.36*cm, 20924.88*cm,
   19200.07*cm, 17584.16*cm, 16072.45*cm,
   14660.38*cm, 13343.46*cm, 12117.33*cm,
   10977.70*cm, 9920.416*cm, 8941.407*cm,
   8036.711*cm, 7202.470*cm, 6434.927*cm,
   5730.429*cm, 5085.425*cm, 4496.467*cm,
   3960.210*cm, 3473.413*cm, 3032.937*cm,
   2635.746*cm, 2278.907*cm, 1959.588*cm,
   1675.064*cm, 1422.710*cm, 1200.004*cm,
   1004.528*cm, 833.9666*cm, 686.1063*cm
 };




  _mptScintillatorMaterial = new G4MaterialPropertiesTable();
  _mptScintillatorMaterial->AddProperty("RAYLEIGH",ENERGY_3, Rayleigh, nEntries3);
  _mptScintillatorMaterial->AddProperty("ABSLENGTH",PhotonEnergy, Absorption1, nEntries2);
#if G4VERSIONNUMBER < 950
  _mptScintillatorMaterial->AddProperty("FASTCOMPONENT",PhotonEnergyScintillatorMaterial, scintFastScintillatorMaterial, nEntries);
#else
  _mptScintillatorMaterial->AddProperty("FASTCOMPONENT",PhotonEnergyScintillatorMaterial, scintFastScintillatorMaterial, nEntries)->SetSpline(true);
#endif
  _mptScintillatorMaterial->AddProperty("RINDEX",PhotonEnergyScintillatorMaterial, RefractiveIndexScintillatorMaterial, nEntries);
  _mptScintillatorMaterial->AddConstProperty("SCINTILLATIONYIELD",8000./MeV); //Approximately correct
  _mptScintillatorMaterial->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  _mptScintillatorMaterial->AddConstProperty("FASTTIMECONSTANT",70.*ns); //Approximately correct
  _mptScintillatorMaterial->AddConstProperty("YIELDRATIO",1.0);

  //Assume rayleigh scattering and absorption same as water


  /*The below is required for mie scattering
  G4double scatteringLength=2.17*um;
  G4double anisotropyFactor=0.800;
  _mptScintillatorMaterial->AddConstProperty("MIEHG",scatteringLength);
  _mptScintillatorMaterial->AddConstProperty("MIEHG_FORWARD",anisotropyFactor); 
  _mptScintillatorMaterial->AddConstProperty("MIEHG_BACKWARD",0.0);
  _mptScintillatorMaterial->AddConstProperty("MIEHG_FORWARD_RATIO",1.0);
  //Absoroption length is irrelevant as the screen should be pretty thin
  //  _mptScintillatorMaterial->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption1,     nEntries)
  //    ->SetSpline(true);
  */
  
  _scintillatorLayerMaterial->SetMaterialPropertiesTable(_mptScintillatorMaterial);
}


void BDSScintillatorScreen::ComputeDimensions(){
  
  //  _xLength = _yLength = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  //  _xLength = std::max(_xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  //  _yLength = std::max(_yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  _screenWidth=1*m;
  _screenHeight=3*cm;

  //Length due to the screen thickness
  _frontThickness=0;//13*um;
  _baseThickness=0;//275*um;
  _backThickness=0;//13*um;
  
  _totalThickness =  
    _frontThickness+
    _scintillatorThickness+
    _baseThickness+
    _backThickness;

  //The scoring plane...
  _scoringPlaneThickness=1*um;

  //Compute the marker volume length according to the screen thickness and width.
  G4double z_wid = _screenWidth * std::sin(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double z_thi = _totalThickness * std::cos(_screenAngle);//Length due to the screen thickness
  G4double x_wid = _screenWidth * std::cos(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double x_thi = _totalThickness * std::sin(_screenAngle);//Length due to the screen thickness
  itsLength = (z_wid + z_thi);
  _xLength = x_wid +x_thi + 2*_scoringPlaneThickness;
  _yLength = _screenHeight;
}

void BDSScintillatorScreen::BuildMarkerVolume(){
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


BDSScintillatorScreen::~BDSScintillatorScreen()
{
  delete itsVisAttributes;
}
