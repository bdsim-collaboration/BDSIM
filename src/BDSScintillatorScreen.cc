/* BDSIM code.    
A scintillator screen.
Work in progress.  
*/

#include "BDSGlobalConstants.hh" 
#include "BDSScintillatorScreen.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include "parser/gmad.h"
#include <map>

//============================================================
typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSScintillatorScreen::BDSScintillatorScreen (G4String aName, G4double aLength, G4double tScint):
  BDSAcceleratorComponent(aName, aLength, 0, 0, 0, SetVisAttributes()),_scintillatorThickness(tScint)
{
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
					0,
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

void BDSScintillatorScreen::BuildScintillatorLayer(){
// The phosphor layer
//   
  G4double dispZ=_frontThickness+(_scintillatorThickness)/2.0-_totalThickness/2.0;
  itsScintillatorLayerSolid = new G4Box("PhosphorLayer",_screenWidth/2.0,_screenHeight/2.0,_scintillatorThickness/2.0);
  itsScintillatorLayerLog = new G4LogicalVolume(itsScintillatorLayerSolid,_scintillatorLayerMaterial,"PhosphorLayer",0,0,0);

  itsScintillatorLayerLog->SetVisAttributes(_visAttScint);
  itsScintillatorLayerPhys=  new G4PVPlacement(0,G4ThreeVector(0,0,dispZ),itsScintillatorLayerLog,"ScreenPhosphorLayer",
					       itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsScintillatorLayerPhys);
}

void BDSScintillatorScreen::BuildBaseLayer(){
  //The PET backing layer
  G4double dispZ=_scintillatorThickness+_frontThickness+_baseThickness/2.0-_totalThickness/2.0;
  itsBaseLayerSolid = new G4Box("PETLayer",_screenWidth/2.0,_screenHeight/2.0,_baseThickness/2.0);
  itsBaseLayerLog = new G4LogicalVolume(itsBaseLayerSolid,BDSMaterials::Instance()->GetMaterial("PET"),"PETLayer",0,0,0);
  
  itsBaseLayerLog->SetVisAttributes(_visAttBase);
  itsBaseLayerPhys =  new G4PVPlacement(0,G4ThreeVector(0,0,dispZ),itsBaseLayerLog,"ScreenPETLayer",
					itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  SetMultiplePhysicalVolumes(itsBaseLayerPhys);
}

void BDSScintillatorScreen::BuildBackLayer(){
  G4double dispZ=_frontThickness+_scintillatorThickness+_baseThickness+_backThickness/2.0-_totalThickness/2.0;
  itsBackLayerSolid = new G4Box("CelluloseBack",_screenWidth/2.0,_screenHeight/2.0,_backThickness/2.0);
  itsBackLayerLog = new G4LogicalVolume(itsBackLayerSolid,BDSMaterials::Instance()->GetMaterial("Cellulose"),"CelluloseBack",0,0,0);
  itsBackLayerLog->SetVisAttributes(_visAttFront);
  itsBackLayerPhys = new G4PVPlacement(0,G4ThreeVector(0,0,dispZ),itsBackLayerLog,"ScreenCelluloseBack",
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
  //Crystal YAG
  _scintillatorLayerMaterial = BDSMaterials::Instance()->GetMaterial("YAG");

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
    G4cout << energy << G4endl;
    PhotonEnergyScintillatorMaterial[i]=energy;
  }
  G4double RefractiveIndexScintillatorMaterial[nEntries] = //Approximately correct, but check for different wavelengths
    { 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82,
      1.82, 1.82 };
  G4double scintFastScintillatorMaterial[nEntries] = //Approximately correct
    { 0, 0.25, 2.0, 14.0, 13.0, 7.0, 4.0, 2.0, 0.0 };
  
  _mptScintillatorMaterial = new G4MaterialPropertiesTable();
  _mptScintillatorMaterial->AddProperty("FASTCOMPONENT",PhotonEnergyScintillatorMaterial, scintFastScintillatorMaterial, nEntries)->SetSpline(true);
  _mptScintillatorMaterial->AddProperty("RINDEX",PhotonEnergyScintillatorMaterial, RefractiveIndexScintillatorMaterial, nEntries);
  _mptScintillatorMaterial->AddConstProperty("SCINTILLATIONYIELD",8000./MeV); //Approximately correct
  _mptScintillatorMaterial->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  _mptScintillatorMaterial->AddConstProperty("FASTTIMECONSTANT",70.*ns); //Approximately correct
  _mptScintillatorMaterial->AddConstProperty("YIELDRATIO",1.0);

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
  /*
  _xLength = _yLength = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  _xLength = std::max(_xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  _yLength = std::max(_yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );
  */

  _xLength=_yLength=10*m;

  _screenWidth=1*m;
  _screenHeight=3*cm;
  _screenAngle=0; //Degrees.
  
  _frontThickness=0;//13*um;
  _baseThickness=0;//275*um;
  _backThickness=0;//13*um;
  
  _totalThickness =  
    _frontThickness+
    _scintillatorThickness+
    _baseThickness+
    _backThickness;
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
