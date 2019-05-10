#include <G4SDParticleWithEnergyFilter.hh>
#include <PSAmbientDose.hh>
#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXY = 1*m;
  G4double world_sizeZ  = 1*m;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");

  G4Box* solidWorld =
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking


  //
  // Shape 2
  //
  G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_Galactic");
  G4ThreeVector pos2 = G4ThreeVector(0, 0*cm, 10*cm);

  // Trapezoid shape
  G4Box* solidShape2 =
    new G4Box("Shape2", 0.5*20*cm, 0.5*20*cm,0.5*20*cm); //its size

  G4LogicalVolume* logicShape2 =
    new G4LogicalVolume(solidShape2,         //its solid
                        shape2_mat,          //its material
                        "Shape2");           //its name

  new G4PVPlacement(0,                       //no rotation
                    pos2,                    //at position
                    logicShape2,             //its logical volume
                    "Shape2",                //its name
                    logicWorld,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking


  // Set Shape2 as scoring volume
  //
  fScoringVolume = logicShape2;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void B1DetectorConstruction::ConstructSDandField()
{

    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    //
    // Sensitive Detector Name
    G4String concreteSDname = "AmbientDose_1";

    //------------------------
    // MultiFunctionalDetector
    //------------------------
    //
    // Define MultiFunctionalDetector with name.
    G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(concreteSDname);
    SDman->AddNewDetector(MFDet);                 // Register SD to SDManager

    SetSensitiveDetector(fScoringVolume,MFDet);

    ///  FILTERS

    /*
    G4String fltName;
    G4double kmin;
    G4double kmax;
    G4SDParticleWithEnergyFilter* neutronFilter = new G4SDParticleWithEnergyFilter(fltName="neutronFilter");
    kmin = 1E-11*MeV;
    kmax = 230*MeV;
    neutronFilter->add("neutron");
    neutronFilter->SetKineticEnergy(kmin,kmax);
    */

    /// Scorer for ambient dose
    //G4PSEnergyDeposit* ps_ambientDose = new G4PSEnergyDeposit("Ambient_dose_det");
    PSAmbientDose* ps_ambientDose = new PSAmbientDose("Ambient_dose_det");
    //ps_ambientDose->SetFilter(neutronFilter);
    MFDet->RegisterPrimitive(ps_ambientDose);

}
