#include "BDSCavity.hh"
#include "BDSCavityFactoryPillBox.hh"
#include "BDSColours.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSCavityFactoryPillBox::BDSCavityFactoryPillBox()
{;}

BDSCavityFactoryPillBox::~BDSCavityFactoryPillBox()
{;}

BDSCavity* BDSCavityFactoryPillBox::CreateCavity(G4String             name,
						 G4double             totalChordLength,
						 const BDSCavityInfo* info,
						 G4Material*          vacuumMaterial)
{
  /*
  //Creates a solid 
  G4VSolid* outerSolid = new G4Tubs(name + "_outer_solid",            // name
				    irisRadius,                       // inner radius
				    cavityRadius + thickness,         // outer radius
				    0.5 * chordLength - lengthSafety, // half length
				    0.0,                              // start angle
				    2*CLHEP::pi);                     // sweep angle
  RegisterSolid(outerSolid);
   
  //Creates a cylinder  to subtract from larger cylinder. 
  innerSolid = new G4Tubs(name + "_inner_solid",        // name
			  0.0,                          // inner radius 
			  cavityRadius,                 // outer radius
			  0.5*chordLength - thickness,  // galf length
			  0.0,                          // star angle
			  2*CLHEP::pi);                 // sweep angle

  //Do the subtraction
  cavitySolid = new G4SubtractionSolid(name + "_cavity_solid",    //name
				       outerSolid,                //solid1
				       innerSolid);               //minus solid2

  //Logical volume from cavity solid
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
				 cavityInfo->material, // material
				 name + "_cavity_lv"); // name
  
  //Vacuum:  Union of two solids.  One cylinder (VacuumInnerCavity) to fill the centre, and a longer,
  //thinner cylinder (vacuumAperture) to fill the ends provided by the thickness.

  G4double vacuumInnerRadius = cavityRadius - thickness - lengthSafety;
  G4double vacuumHalfLength  = 0.5*chordLength - thickness - lengthSafety;
  G4VSolid* vacuumInnerCavity = new G4Tubs(name + "_vacuum_inner_cavity_solid",// name
					   0.0,                                // inner radius
					   vacuumInnerRadius,                  // outer radius
					   vacuumHalfLength,                   // half length
					   0.0,                                // start angle
					   2*CLHEP::pi);                       // sweep angle

  G4VSolid* vacuumAperture = new G4Tubs(name + "_vacuum_aperture_solid",   // name
					0.0,                               // inner radius
					irisRadius - lengthSafety,         // outer radius
					0.5 * chordLength - lengthSafety,  // half length
					0.0,                               // start angle
					2*CLHEP::pi);                      // sweep angle
  
  // Create the vacuum as a union of the two solides defined prior
  vacuumSolid = new G4UnionSolid(name + "_vacuum_solid",  // name
				 vacuumInnerCavity,       // solid one
				 vacuumAperture);         // added to solid two.

  // Logical volume from the solid.
  vacuumLV = new G4LogicalVolume(vacuumSolid,                // solid
				 cavityInfo->vacuumMaterial, // material
				 name + "_vacuum_lv");       // name

  // visualisation attributes
  auto col = BDSColours::Instance()->GetColour("srfcavity");
  G4VisAttributes* cavityVis = new G4VisAttributes(*col);
  cavityVis->SetVisibility(true);
  cavityVis->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
  cavityLV->SetVisAttributes(cavityVis);
  RegisterVisAttributes(cavityVis);
  
  //vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
  */
  return nullptr;
}
