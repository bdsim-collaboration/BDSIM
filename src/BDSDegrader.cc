#include "BDSAcceleratorComponent.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDegrader.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"


#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4ExtrudedSolid.hh"



#include "globals.hh" // geant4 globals / types

BDSDegrader::BDSDegrader (G4String   name, 
                    G4double   length,
                    G4double   outerDiameter,
                    G4String   degraderMaterial,
                    G4int      numberWedges,
                    G4double   wedgeBasewidth,
                    G4double   wedgeHeight,
                    G4double   degraderHeight,
                    G4double   materialThickness):
BDSAcceleratorComponent(name, length, 0, "degrader")
{;}

BDSDegrader::~BDSDegrader()
{;}

void BDSDegrader::BuildContainerLogicalVolume()
{
        //WRONG G4Box* containerSolid = 
    // containerSolid = new G4Boxsjfdlkdsjfl
    // build solid - G4Box or G4Tubs - called containerSolid
    // build logical volume - use BDSGlobalConstants::Instance()->GetEmptyMaterial()
    // should be called containerLV
    
    containerSolid = new G4Box(name + "_container_solid",
                            outerDiameter*0.5,
                            outerDiameter*0.5,
                            chordLength*0.5);
    
    G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
    
    containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                      emptyMaterial,
                                      name + "_container_lv");
}


void BDSDegrader::Build()
{  

    G4VSolid* boxSolid = new G4Box(name + "_outer_solid",
                                   outerDiameter*0.5 - lengthSafety,
                                   outerDiameter*0.5 - lengthSafety,
                                   chordLength*0.5 - lengthSafety);
    
    RegisterSolid(boxSolid);
    
    G4Material* material = BDSMaterials::Instance()->GetMaterial(degraderMaterial);
    
    G4LogicalVolume* degraderLV = new G4LogicalVolume(boxSolid,                 // solid
                                                        material,               // material
                                                        name + "_degrader_lv"); // name
    RegisterLogicalVolume(degraderLV);
    
    
    G4VisAttributes* degraderVisAttr = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
    degraderLV->SetVisAttributes(degraderVisAttr);
    RegisterVisAttributes(degraderVisAttr);
    
    G4PVPlacement* degPV = new G4PVPlacement(0,                         // rotation
                                              (G4ThreeVector)0,         // position
                                              degraderLV,               // its logical volume
                                              name + "_degrader_pv",    // its name
                                              containerLogicalVolume,   // its mother  volume
                                              false,                    // no boolean operation
                                              0,                        // copy number  
                                              checkOverlaps);
    
    RegisterPhysicalVolume(degPV);

    
    // solid
    // logical volume
    // vis attributes
    // user limits
    // place logical volume in containerLV

}
