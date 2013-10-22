/* BDSIM code.    Version beta
   Author: Grahame I. Agapov, Royal Holloway, Univ. of London.

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

BDSScintillatorScreen::BDSScintillatorScreen (G4String aName)
  BDSAcceleratorComponent(aName), 
  itsSoilTube(NULL),itsTunnelTube(NULL),  itsInnerTunnelTube(NULL), itsInnerTunnelLogicalVolume(NULL),
  itsSoilTunnelLogicalVolume(NULL), itsTunnelUserLimits(NULL), itsSoilTunnelUserLimits(NULL),
  itsInnerTunnelUserLimits(NULL), itsVisAttributes(NULL), itsEqRhs(NULL),
  itsCollimatorMaterial(CollimatorMaterial), itsOuterR(outR)
{
  if ( (*LogVolCount)[itsName]==0)
    {
      ComputeDimensions();
      BuildMarkerVolume();
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
	BuildTunnel();
      }
      BuildScintillatorScreen();
      SetVisAttributes();
      
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=GetMarkerLogicalVolume();
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }  
}

void BDSScintillatorScreen::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceSolid(true);
}

void BDSScintillatorScreen::BuildFrontLayer(){
  //The cellulose protective layer
  G4Box* celluloseFront_box = new G4Box("CelluloseFront",screen_x,screen_y,_frontThickness);
  G4LogicalVolume* celluloseFront_log= new G4LogicalVolume(celluloseFront_box,BDSMaterials::Instance()->GetMaterial("Cellulose"),"CelluloseFront",0,0,0);
  G4double dispZ=_frontThickness/2.0-_totalThickness/2.0;
  G4VPhysicalVolume* celluloseFront_phys = new G4PVPlacement(0,G4ThreeVector(0,0,dispZ),celluloseFront_log,"ScreenCelluloseFront",
							     GetMarkerLogicalVolume(),false,0);
}

void BDSScintillatorScreen::BuildScintillatorLayer(){
// The phosphor layer
//   
  G4double dispZ=(_frontThickness+_scintillatorThickness)/2.0-_totalThickness/2.0;
  G4Box* phosphorLayer_box = new G4Box("PhosphorLayer",screen_x,screen_y,_scintillatorThickness);
  G4LogicalVolume* phosphorLayer_log= new G4LogicalVolume(phosphorLayer_box,YAGScreenMaterial,"PhosphorLayer",0,0,0);

G4VPhysicalVolume* phosphorLayer_phys=  new G4PVPlacement(0,G4ThreeVector(0,0,dispZ),phosphorLayer_log,"ScreenPhosphorLayer",
		    GetMarkerLogicalVolume(),false,0);
}

void BDSScintillatorScreen::BuildBaseLayer(){
  //The PET backing layer
  G4double dispZ=(_frontThickness+_scintillatorThickness+_baseThickness)/2.0-_totalThickness/2.0;
  G4Box* PETLayer_box = new G4Box("PETLayer",screen_x,screen_y,PETThickness);
  G4LogicalVolume* PETLayer_log= new G4LogicalVolume(PETLayer_box,PET,"PETLayer",0,0,0);
  
  G4VPhysicalVolume* PETLayer_phys =  new G4PVPlacement(0,G4ThreeVector(0,0,dispZ),PETLayer_log,"ScreenPETLayer",
		    GetMarkerLogicalVolume(),false,0);
}

void BDSScintillatorScreen::BuildBackLayer(){
  G4double dispZ=(_frontThickness+_scintillatorThickness+_baseThickness+_backThickness)/2.0-_totalThickness/2.0;
  G4Box* celluloseBack_box = new G4Box("CelluloseBack",screen_x,screen_y,celluloseThickness);
  G4LogicalVolume* celluloseBack_log= new G4LogicalVolume(celluloseBack_box,Cellulose,"CelluloseBack",0,0,0);

  new G4PVPlacement(0,G4ThreeVector(0,0,dispZ),celluloseBack_log,"ScreenCelluloseBack",
		    GetMarkerLogicalVolume(),false,0);
}

void BDSScintillatorScreen::BuildOpticalSurfaces(){
  //Add the optical surfaces.
  //Mirrored surface between the phosphor and the PET backing.
  G4OpticalSurface* OpSurface=new G4OpticalSurface("OpSurface");
  G4LogicalBorderSurface* Surface = new 
    G4LogicalBorderSurface("phosphor_PET_surface", phosphorLayer_phys, PETLayer_phys, OpSurface);
  G4double sigma_alpha=1.0;
  //TiO2 particles provide the reflectivity in the PET layer hence dielectric-metal
  OpSurface -> SetType(dielectric_metal);
  OpSurface -> SetModel(unified);
  OpSurface -> SetFinish(polished);
  OpSurface -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
  SMPT->AddConstProperty("REFLECTIVITY",0.8);
  SMPT->AddConstProperty("RINDEX",1.5750);
  OpSurface->SetMaterialPropertiesTable(SMPT);


  //Surface between the phosphor and the cellulose front.
  G4OpticalSurface* OpSurface2=new G4OpticalSurface("OpSurface2");
  G4LogicalBorderSurface* Surface2 = new 
    G4LogicalBorderSurface("phosphor_cellulose_surface", phosphorLayer_phys, celluloseFront_phys, OpSurface2);
  OpSurface2 -> SetType(dielectric_dielectric);
  OpSurface2 -> SetModel(unified);
  OpSurface2 -> SetFinish(ground);
  OpSurface2 -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT2 = new G4MaterialPropertiesTable();
  SMPT2->AddConstProperty("REFLECTIVITY",0.0);
  SMPT2->AddConstProperty("RINDEX",1.532);
  OpSurface2->SetMaterialPropertiesTable(SMPT2);

  //Surface between the cellulose and the experimental hall.
  G4OpticalSurface* OpSurface3=new G4OpticalSurface("OpSurface3");
  G4LogicalBorderSurface* Surface3 = new 
    G4LogicalBorderSurface("cellulose_hall_surface", celluloseFront_phys, expHall_phys, OpSurface3);
  OpSurface3 -> SetType(dielectric_dielectric);
  OpSurface3 -> SetModel(unified);
  OpSurface3 -> SetFinish(ground);
  OpSurface3 -> SetSigmaAlpha(sigma_alpha);
  G4MaterialPropertiesTable* SMPT3 = new G4MaterialPropertiesTable();
  SMPT3->AddConstProperty("REFLECTIVITY",0.0);
  SMPT3->AddConstProperty("RINDEX",1.0);
  OpSurface3->SetMaterialPropertiesTable(SMPT3);
}


void BDSScintillatorScreen::BuildScintillatorScreen()
{
  BuildScintillatorMaterial();
  BuildFrontLayer();
  BuildScintillatorLayer();
  BuildBaseLayer();
  BuildBackLayer();
  BuildOpticalSurfaces();
  
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    SetSensitiveVolume(itsScintillatorLayerLog);
  }
  SetMultiplePhysicalVolumes(itsPhysiComp);
  G4cout << "BDSScintillatorScreen: finished building geometry" << G4endl;
}

BDSScintillatorScreen::BuildScintillatorMaterial(){
  BuildScintillatorCompound();
  BuildScintillatorOpticalProperties();
}

BDSScintillatorScreen::BuildScintillatorCompound(){
  //Scintillator grains sudpended in a polyurethane elastomer with a specific fill factor
  G4double fill_factor=0.5; //i.e. fraction by volume
  G4double yag_screen_density=fill_factor*BDSMaterials::Instance()->GetMaterial("YAG")->GetDensity()+(1-fill_factor)*BDSMaterials::Instance()->GetMaterial("Polyurethane")->GetDensity();
  G4double yag_fraction_by_mass=fill_factor*BDSMaterials::Instance()->GetMaterial("YAG")->GetDensity()/yag_screen_density;
  G4double pur_fraction_by_mass=1-yag_fraction_by_mass;
  _scintillatorMaterial = new G4Material("ScintillatorMaterial", density=yag_screen_density, nelements=2);
  _scintillatorMaterial->AddMaterial(BDSMaterials::Instance()->GetMaterial("YAG"), yag_fraction_by_mass);
  _scintillatorMaterial->AddMaterial(BDSMaterials::Instance()->GetMaterial("Polyurethane"), pur_fraction_by_mass);
}

BDSScintillatorScreen::BuildScintillatorOpticalProperties(){
  //Fill the material properties table of the _scintillatorMaterial

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
  
  mptScintillatorMaterial->AddProperty("FASTCOMPONENT",PhotonEnergyScintillatorMaterial, scintFastScintillatorMaterial, nEntries)->SetSpline(true);
  mptScintillatorMaterial->AddProperty("RINDEX",PhotonEnergyScintillatorMaterial, RefractiveIndexScintillatorMaterial, nEntries);
    
  G4MaterialPropertiesTable*mptScintillatorMaterial = new G4MaterialPropertiesTable();
  mptScintillatorMaterial->AddConstProperty("SCINTILLATIONYIELD",8000./MeV); //Approximately correct
  mptScintillatorMaterial->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  mptScintillatorMaterial->AddConstProperty("FASTTIMECONSTANT",70.*ns); //Approximately correct
  mptScintillatorMaterial->AddConstProperty("YIELDRATIO",1.0);
  G4double scatteringLength=2.17*um;
  G4double anisotropyFactor=0.800;
  mptScintillatorMaterial->AddConstProperty("MIEHG",scatteringLength);
  mptScintillatorMaterial->AddConstProperty("MIEHG_FORWARD",anisotropyFactor); 
  mptScintillatorMaterial->AddConstProperty("MIEHG_BACKWARD",0.0);
  mptScintillatorMaterial->AddConstProperty("MIEHG_FORWARD_RATIO",1.0);
  //Absoroption length is irrelevant as the screen should be pretty thin
  //  mptYAGScreenMaterial->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption1,     nEntries)
  //    ->SetSpline(true);
  
  _scintillatorMaterial->SetMaterialPropertiesTable(mptScintillatorMaterial);
}


BDSScintillatorScreen::ComputeDimensions(){
  xLength = yLength = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  xLength = std::max(xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  yLength = std::max(yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  _screenWidth=1*m;
  _screenHeight=3*cm;
  _screenAngle=0; //Degrees.
  
  _frontThickness=13*mum;
  _scintillatorThickness=300*mum;
  _baseThickness=275*mum;
  _backThickness=13*mum;
  
  _totalThickness =  
    _frontThickness+
    _scintillatorThickness+
    _baseThickness+
    _backThickness;

  itsLength=_frontThickness+_scintillatorThickness+_baseThickness+_backThickness


}

BDSScintillatorScreen::BuildMarkerVolume(){
  itsMarkerLogicalVolume=new G4LogicalVolume
    (new G4Box( itsName+"_marker_log",
                xLength,
		yLength,
		(itsLength+BDSGlobalConstants::Instance()->GetLengthSafety())/2), //z half length 
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     itsName+"_log");
}


BDSScintillatorScreen::~BDSScintillatorScreen()
{
  delete itsVisAttributes;
}
