#include "BDSAcceleratorComponent.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDegrader.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4ExtrudedSolid.hh"

#include "globals.hh" // geant4 globals / types

BDSDegrader::BDSDegrader (G4String   name, 
                    G4double   length,
                    G4double   outerDiameterIn,
                    G4String   degraderMaterialIn,
                    G4int      numberWedgesIn,
                    G4double   wedgeBasewidthIn,
                    G4double   wedgeHeightIn,
                    G4double   degraderHeightIn,
                    G4double   materialThicknessIn):
BDSAcceleratorComponent(name, length, 0, "degrader"),
    outerDiameter(outerDiameterIn),
    degraderMaterial(degraderMaterialIn),
    numberWedges(numberWedgesIn),
    wedgeBasewidth(wedgeBasewidthIn),
    wedgeHeight(wedgeHeightIn),
    degraderHeight(degraderHeightIn),
    materialThickness(materialThicknessIn)
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
    BDSAcceleratorComponent::Build();
    
    G4Material* material = BDSMaterials::Instance()->GetMaterial(degraderMaterial);
    
    wedgeBasewidth = chordLength/numberWedges - lengthSafety; 
    
    //Full wedge vertex co-ordinates
    std::vector<G4TwoVector> fullWedgeSide; 
    fullWedgeSide.push_back( G4TwoVector(0, wedgeBasewidth*0.5) );
    fullWedgeSide.push_back( G4TwoVector(wedgeHeight, 0) );
    fullWedgeSide.push_back( G4TwoVector(0, -0.5 *wedgeBasewidth) );
     
    G4ExtrudedSolid* fullWedge = new G4ExtrudedSolid(name + "_outer_solid",
                                    fullWedgeSide,
                                    degraderHeight*0.5,
                                    G4TwoVector(),1, G4TwoVector(), 1);
    
    RegisterSolid(fullWedge);
    
    G4LogicalVolume* fullWedgeLV = new G4LogicalVolume(fullWedge,              // solid
                                                      material,               // material
                                                      name + "_degrader_lv"); // name 
    RegisterLogicalVolume(fullWedgeLV);
    
    
    
    //Half wedge vertex co-ordinates
    std::vector<G4TwoVector> halfWedgeSide; 
    halfWedgeSide.push_back( G4TwoVector(0, wedgeBasewidth*0.5) );
    halfWedgeSide.push_back( G4TwoVector(wedgeHeight, 0) );
    halfWedgeSide.push_back( G4TwoVector(0, 0) );
    
    G4ExtrudedSolid* halfWedge = new G4ExtrudedSolid(name + "_outer_solid",
                                                     halfWedgeSide,
                                                     degraderHeight*0.5,
                                                     G4TwoVector(),1, G4TwoVector(), 1);
    
    RegisterSolid(halfWedge);
    
    G4LogicalVolume* halfWedgeLV = new G4LogicalVolume(halfWedge,             // solid
                                                      material,               // material
                                                      name + "_degrader_lv"); // name     
    RegisterLogicalVolume(halfWedgeLV);

    
    G4double theta = atan(wedgeBasewidth / (2.0*wedgeHeight));
    G4double overlap = (materialThickness/numberWedges - wedgeBasewidth) - (sin(M_PI/2.0 - theta) / sin(theta));
    
    G4double xoffsetLeft = overlap * -0.5;
    G4double xoffsetRight = overlap * 0.5;

    
    G4double zoffsetRightFullPos = wedgeBasewidth*0.5 + 0.5*lengthSafety;
    G4double zoffsetRightHalfPos = wedgeBasewidth*0.5 + 2.0*wedgeBasewidth + 2.5*lengthSafety;
    G4double zoffsetRightFullNeg = -1.0*wedgeBasewidth - 0.5*wedgeBasewidth - 1.5*lengthSafety;

    
    G4double zoffsetLeftFullPos = wedgeBasewidth*0.5 + 1.0*wedgeBasewidth + 1.5*lengthSafety;
    G4double zoffsetLeftFullNeg = wedgeBasewidth*-0.5 - 0.5*lengthSafety;
    G4double zoffsetLeftHalfNeg = -2.0*wedgeBasewidth - 0.5*wedgeBasewidth - 2.5*lengthSafety;    
    
    
    G4RotationMatrix* xRot = new G4RotationMatrix;  
    xRot->rotateX(M_PI/2.0);                     
    RegisterRotationMatrix(xRot);
    
    /*
    G4VisAttributes* degraderVisAttr = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
    halfWedgeLV->SetVisAttributes(degraderVisAttr);
    fullWedgeLV->SetVisAttributes(degraderVisAttr);

    RegisterVisAttributes(degraderVisAttr);
    */
    
    G4ThreeVector transLeftFullPos(xoffsetLeft, 0, zoffsetLeftFullPos);
    G4ThreeVector transLeftFullNeg(xoffsetLeft, 0, zoffsetLeftFullNeg);
    G4ThreeVector transLeftHalfNeg(xoffsetLeft, 0, zoffsetLeftHalfNeg);

    G4ThreeVector transRightFullPos(xoffsetRight, 0, zoffsetRightFullPos);
    G4ThreeVector transRightHalfPos(xoffsetRight, 0, zoffsetRightHalfPos);
    G4ThreeVector transRightFullNeg(xoffsetRight, 0, zoffsetRightFullNeg);

    
    //G4ThreeVector halfwedgeTrans(xoffset, 0, 2.0*(wedgeBasewidth+lengthSafety));
    
    
    G4PVPlacement* leftFullPosPV = new G4PVPlacement(xRot,         // rotation
                                            transLeftFullPos,           // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_degrader_pv",    // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);

    G4PVPlacement* leftFullNegPV = new G4PVPlacement(xRot,         // rotation
                                            transLeftFullNeg,           // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_degrader_pv",    // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
    G4PVPlacement* leftHalfNegPV = new G4PVPlacement(xRot,         // rotation
                                            transLeftHalfNeg,           // position
                                            halfWedgeLV,              // its logical volume
                                            name + "_degrader_pv",    // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
    
        
    G4PVPlacement* rightFullPosPV = new G4PVPlacement(xRot,           // rotation
                                            transRightFullPos,                   // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_degrader_pv",    // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
    G4PVPlacement* rightHalfPosPV = new G4PVPlacement(xRot,           // rotation
                                            transRightHalfPos,                   // position
                                            halfWedgeLV,              // its logical volume
                                            name + "_degrader_pv",    // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
    G4PVPlacement* rightFullNegPV = new G4PVPlacement(xRot,           // rotation
                                            transRightFullNeg,                   // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_degrader_pv",    // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
    
    RegisterPhysicalVolume(leftFullPosPV);
    RegisterPhysicalVolume(leftFullNegPV);
    RegisterPhysicalVolume(leftHalfNegPV);

    RegisterPhysicalVolume(rightFullPosPV);
    RegisterPhysicalVolume(rightHalfPosPV);
    RegisterPhysicalVolume(rightFullNegPV);
    
    
    // solid
    // logical volume
    // vis attributes
    // user limits
    // place logical volume in containerLV

}
