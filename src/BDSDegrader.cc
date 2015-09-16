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
                    G4double   wedgeHeightIn,
                    G4double   degraderHeightIn,
                    G4double   materialThicknessIn):
BDSAcceleratorComponent(name, length, 0, "degrader"),
    outerDiameter(outerDiameterIn),
    degraderMaterial(degraderMaterialIn),
    numberWedges(numberWedgesIn),
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
    
  //Full wedge 
  std::vector<G4TwoVector> fullWedgeSide;  //vertex co-ordinates
  fullWedgeSide.push_back( G4TwoVector(0, wedgeBasewidth*0.5) );
  fullWedgeSide.push_back( G4TwoVector(wedgeHeight, 0) );
  fullWedgeSide.push_back( G4TwoVector(0, -0.5 *wedgeBasewidth) );
     
  G4ExtrudedSolid* fullWedge = new G4ExtrudedSolid(name + "_fullwedge_solid",
                                                    fullWedgeSide,
                                                    degraderHeight*0.5,
                                                    G4TwoVector(),1, G4TwoVector(), 1);
    
  RegisterSolid(fullWedge);
    
  G4LogicalVolume* fullWedgeLV = new G4LogicalVolume(fullWedge,                 // solid
                                                      material,                 // material
                                                      name + "_fullwedge_lv");  // name 
  RegisterLogicalVolume(fullWedgeLV);
    
    
    
  //Half wedge 
  std::vector<G4TwoVector> halfWedgeSide; //vertex co-ordinates
  halfWedgeSide.push_back( G4TwoVector(0, wedgeBasewidth*0.5) );
  halfWedgeSide.push_back( G4TwoVector(wedgeHeight, 0) );
  halfWedgeSide.push_back( G4TwoVector(0, 0) );
    
  G4ExtrudedSolid* halfWedge = new G4ExtrudedSolid(name + "_halfwedge_solid",
                                                    halfWedgeSide,
                                                    degraderHeight*0.5,
                                                    G4TwoVector(),1, G4TwoVector(), 1);
    
  RegisterSolid(halfWedge);
    
  G4LogicalVolume* halfWedgeLV = new G4LogicalVolume(halfWedge,                 // solid
                                                      material,                 // material
                                                      name + "_halfwedge_lv");  // name     
  RegisterLogicalVolume(halfWedgeLV);

  
  //Angle between hypotenuse and height (in the triangular wedge face)
  G4double theta = atan(wedgeBasewidth / (2.0*wedgeHeight));       
    
  //Overlap distance of wedges
  G4double overlap = (materialThickness/numberWedges - wedgeBasewidth) * (sin(M_PI/2.0 - theta) / sin(theta));
  
  //Offsets for wedge overlap
  G4double xoffsetLeft = overlap * -0.5;
  G4double xoffsetRight = overlap * 0.5;

  //Wedge positions
  G4double zoffsetRightFullPos = wedgeBasewidth*0.5 + 0.5*lengthSafety;
  G4double zoffsetRightHalfPos = wedgeBasewidth*0.5 + 2.0*wedgeBasewidth + 2.5*lengthSafety;
  G4double zoffsetRightFullNeg = -1.0*wedgeBasewidth - 0.5*wedgeBasewidth - 1.5*lengthSafety;

  G4double zoffsetLeftFullPos = wedgeBasewidth*0.5 + 1.0*wedgeBasewidth + 1.5*lengthSafety;
  G4double zoffsetLeftFullNeg = wedgeBasewidth*-0.5 - 0.5*lengthSafety;
  G4double zoffsetLeftHalfNeg = -2.0*wedgeBasewidth - 0.5*wedgeBasewidth - 2.5*lengthSafety;    
    

  //Rotation  of wedges. Left taken to be +VE x direction, right is -VE x direction.
  G4RotationMatrix* rightRot = new G4RotationMatrix;  
  rightRot->rotateX(M_PI/2.0);  
  RegisterRotationMatrix(rightRot);

  G4RotationMatrix* leftRot = new G4RotationMatrix;  
  leftRot->rotateX(M_PI/2.0);
  leftRot->rotateZ(M_PI);
  RegisterRotationMatrix(leftRot);

    
  //Wedge color
  G4VisAttributes* degraderVisAttr = new G4VisAttributes(G4Colour(0.625,0.625,0.625));
  halfWedgeLV->SetVisAttributes(degraderVisAttr);
  fullWedgeLV->SetVisAttributes(degraderVisAttr);

  RegisterVisAttributes(degraderVisAttr);
    

  //Translation of individual wedge components
  G4ThreeVector transLeftFullPos(xoffsetLeft, 0, zoffsetLeftFullPos);
  G4ThreeVector transLeftFullNeg(xoffsetLeft, 0, zoffsetLeftFullNeg);
  G4ThreeVector transLeftHalfNeg(xoffsetLeft, 0, zoffsetLeftHalfNeg);

  G4ThreeVector transRightFullPos(xoffsetRight, 0, zoffsetRightFullPos);
  G4ThreeVector transRightHalfPos(xoffsetRight, 0, zoffsetRightHalfPos);
  G4ThreeVector transRightFullNeg(xoffsetRight, 0, zoffsetRightFullNeg);

    
  //Placement of individual wedge components
  G4PVPlacement* leftFullPosPV = new G4PVPlacement(leftRot,           // rotation
                                            transLeftFullPos,         // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_leftfullpos_pv", // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);

  G4PVPlacement* leftFullNegPV = new G4PVPlacement(leftRot,           // rotation
                                            transLeftFullNeg,         // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_leftfullneg_pv", // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
  G4PVPlacement* leftHalfNegPV = new G4PVPlacement(leftRot,           // rotation
                                            transLeftHalfNeg,         // position
                                            halfWedgeLV,              // its logical volume
                                            name + "_lefthalfneg_pv", // its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
    
        
  G4PVPlacement* rightFullPosPV = new G4PVPlacement(rightRot,         // rotation
                                            transRightFullPos,        // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_rightfullpos_pv",// its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
  G4PVPlacement* rightHalfPosPV = new G4PVPlacement(rightRot,         // rotation
                                            transRightHalfPos,        // position
                                            halfWedgeLV,              // its logical volume
                                            name + "_righthalfpos_pv",// its name
                                            containerLogicalVolume,   // its mother  volume
                                            false,                    // no boolean operation
                                            0,                        // copy number  
                                            checkOverlaps);
    
  G4PVPlacement* rightFullNegPV = new G4PVPlacement(rightRot,         // rotation
                                            transRightFullNeg,        // position
                                            fullWedgeLV,              // its logical volume
                                            name + "_rightfullneg_pv",// its name
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

}
