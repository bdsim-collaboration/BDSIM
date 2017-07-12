#include "BDSCavity.hh"
#include "BDSCavityFactoryRectangular.hh"
#include "BDSCavityInfo.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSCavityFactoryRectangular::BDSCavityFactoryRectangular()
{;}

BDSCavityFactoryRectangular::~BDSCavityFactoryRectangular()
{;}

BDSCavity* BDSCavityFactoryRectangular::CreateCavity(G4String             name,
						     G4double             totalChordLength,
						     const BDSCavityInfo* info,
						     G4Material*          vacuumMaterial)
{
  CleanUp();

  G4double thickness    = info->thickness;
  G4double chordLength  = totalChordLength;
  G4double cavityRadius = info->equatorRadius;
  G4double outerBoxHalf = cavityRadius + thickness;
  G4VSolid* outerSolid = new G4Box(name + "_outer_solid",           // name
				   outerBoxHalf,                    // dx
				   outerBoxHalf,                    // dy
				   0.5*chordLength - lengthSafety); // dz
  G4VSolid* sub = new G4Box(name + "_outer_sub_solid",    // name
			    outerBoxHalf - thickness,     // dx
			    outerBoxHalf - thickness,     // dy
			    0.5*chordLength - thickness); // dz
  allSolids.push_back(outerSolid);
  allSolids.push_back(sub);
  
  G4VSolid* innerSolid = new G4Tubs(name + "_inner_solid",  // name
				    0.0,                    // inner radius 
				    info->irisRadius,       // outer radius
				    chordLength,            // galf length
				    0.0,                    // star angle
				    CLHEP::twopi);          // sweep angle
  allSolids.push_back(innerSolid);
  
  // subtraction
  G4VSolid* cavitySolid1 = new G4SubtractionSolid(name + "_cavity_sub_solid",//name
						  outerSolid,                //solid1
						  sub);               //minus solid2
  allSolids.push_back(cavitySolid1);
  G4VSolid* cavitySolid = new G4SubtractionSolid(name + "_cavity_solid", // name
						 cavitySolid1,
						 innerSolid);
  
  // logical volume from cavity solid
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
				 info->material,       // material
				 name + "_cavity_lv"); // name
  allLogicalVolumes.push_back(cavityLV);
  
  // vacuum: union of two solid - one cylinder (VacuumInnerCavity) to fill the centre,
  // and a longer, thinner cylinder (vacuumAperture) to fill the ends provided by the thickness.

  G4double vacuumHalfWidth    = outerBoxHalf - thickness - lengthSafety;
  G4VSolid* vacuumInnerCavity = new G4Box(name + "_vacuum_inner_cavity_solid",// name
					  vacuumHalfWidth,
					  vacuumHalfWidth,
					  0.5*chordLength - thickness - lengthSafety);

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

  G4double outerRadius = cavityRadius + thickness + lengthSafety;
  
  return CommonConstruction(name, vacuumSolid, vacuumMaterial, chordLength, outerRadius);
}

void BDSCavityFactoryRectangular::BuildContainerLogicalVolume(G4String name,
							      G4double chordLength,
							      G4double outerRadius) 
{
  containerSolid = new G4Box(name + "_container_solid",   // name
			     outerRadius,                 // innerRadius
			     outerRadius,                 // outerRadius
			     chordLength*0.5);            // half length
  
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
}
