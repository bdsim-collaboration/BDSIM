#include "BDSCavity.hh"
#include "BDSCavityFactoryPillBox.hh"
#include "BDSCavityInfo.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
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
  CleanUp();
  
  G4double chordLength  = totalChordLength;
  G4double cavityRadius = info->equatorRadius;
  G4VSolid* outerSolid = new G4Tubs(name + "_outer_solid",          // name
				    info->irisRadius,               // inner radius
				    cavityRadius + info->thickness, // outer radius
				    0.5*chordLength - lengthSafety, // half length
				    0.0,                            // start angle
				    CLHEP::twopi);                  // sweep angle
  allSolids.push_back(outerSolid);
   
  // creates a cylinder  to subtract from larger cylinder. 
  G4VSolid* innerSolid = new G4Tubs(name + "_inner_solid",            // name
				    0.0,                              // inner radius 
				    cavityRadius,                     // outer radius
				    0.5*chordLength - info->thickness,// half length
				    0.0,                              // star angle
				    CLHEP::twopi);                    // sweep angle
  allSolids.push_back(innerSolid);
  
  // subtraction
  G4VSolid* cavitySolid = new G4SubtractionSolid(name + "_cavity_solid",    //name
						 outerSolid,                //solid1
						 innerSolid);               //minus solid2
  allSolids.push_back(cavitySolid);
  
  // logical volume from cavity solid
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
				 info->material,       // material
				 name + "_cavity_lv"); // name
  allLogicalVolumes.push_back(cavityLV);
  
  // vacuum: union of two solid - one cylinder (VacuumInnerCavity) to fill the centre,
  // and a longer, thinner cylinder (vacuumAperture) to fill the ends provided by the thickness.

  G4double vacuumInnerRadius = cavityRadius - info->thickness - lengthSafety;
  G4double vacuumHalfLength  = 0.5*chordLength - info->thickness - lengthSafety;
  G4VSolid* vacuumInnerCavity = new G4Tubs(name + "_vacuum_inner_cavity_solid",// name
					   0.0,                                // inner radius
					   vacuumInnerRadius,                  // outer radius
					   vacuumHalfLength,                   // half length
					   0.0,                                // start angle
					   CLHEP::twopi);                      // sweep angle

  G4VSolid* vacuumAperture = new G4Tubs(name + "_vacuum_aperture_solid",   // name
					0.0,                               // inner radius
					info->irisRadius - lengthSafety,   // outer radius
					0.5*chordLength - lengthSafety,    // half length
					0.0,                               // start angle
					CLHEP::twopi);                     // sweep angle
  
  // create the vacuum as a union of the two solides defined prior
  G4VSolid* vacuumSolid = new G4UnionSolid(name + "_vacuum_solid",  // name
					   vacuumInnerCavity,       // solid one
					   vacuumAperture);         // added to solid two.
  allSolids.push_back(vacuumInnerCavity);
  allSolids.push_back(vacuumAperture);
  allSolids.push_back(vacuumSolid);

  // logical volume from the solid. vacuumLV is base class memeber
  vacuumLV = new G4LogicalVolume(vacuumSolid,           // solid
				 vacuumMaterial,        // material
				 name + "_vacuum_lv");  // name
  allLogicalVolumes.push_back(vacuumLV);

  SetUserLimits(chordLength, allLogicalVolumes);

  G4double outerRadius = cavityRadius + info->thickness + lengthSafety;
  BuildContainerLogicalVolume(name, chordLength, outerRadius);

  SetVisAttributes();
  PlaceComponents(name);

  BDSExtent ext = BDSExtent(outerRadius, outerRadius,  chordLength*0.5);

  BDSCavity* result = BuildCavityAndRegisterObjects(ext);

  return result;
}
