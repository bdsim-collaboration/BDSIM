#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSCollimatorBase.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"

#include <map>

BDSCollimatorBase::BDSCollimatorBase(G4String name,
				     G4double length,
				     G4double outerDiameterIn,
				     G4String type,
				     G4double xApertureIn,
				     G4double yApertureIn,
				     G4String collimatorMaterialIn,
				     G4String vacuumMaterialIn):
  BDSAcceleratorComponent(name, length, 0, type),
  outerDiameter(outerDiameterIn),
  xAperture(xApertureIn),
  yAperture(yApertureIn),
  collimatorMaterial(collimatorMaterialIn),
  vacuumMaterial(vacuumMaterialIn)
{
  if(outerDiameter==0)
    {outerDiameter = BDSGlobalConstants::Instance()->GetOuterDiameter();}

  if ( (xAperture > 0.5*outerDiameter) || (yAperture > 0.5*outerDiameter) )
    {
      G4cerr << __METHOD_NAME__ << "half aperture bigger than diameter!" << G4endl;
      G4cerr << "Outer diameter is " << outerDiameter << " mm" << G4endl;
      exit(1);
    }

  if (collimatorMaterialIn == "")
    {
      G4cout << __METHOD_NAME__ << "Warning - no material set for collimator - using copper" << G4endl;
      collimatorMaterial = "Copper";
    }

  collimatorSolid = NULL;
}

void BDSCollimatorBase::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name + "_container_solid",
			     outerDiameter*0.5,
			     outerDiameter*0.5,
			     chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSCollimatorBase::Build()
{
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container
  
  // now build the collimator
  G4VSolid* outerSolid = new G4Box(name + "_outer_solid",
				   outerDiameter * 0.5 - lengthSafety,
				   outerDiameter * 0.5 - lengthSafety,
				   chordLength*0.5 - lengthSafety);

  // only do subtraction if aperture actually set
  G4bool buildVacuumAndAperture = (BDS::IsFinite(xAperture) && BDS::IsFinite(yAperture));
  if(buildVacuumAndAperture)
    {
      BuildInnerCollimator();

      collimatorSolid = new G4SubtractionSolid(name + "_collimator_solid", // name
					       outerSolid,                 // solid 1
					       innerSolid);                // minus solid 2
    }
  else
    {collimatorSolid = outerSolid;}

  G4Material* material = BDSMaterials::Instance()->GetMaterial(collimatorMaterial);
  G4LogicalVolume* collimatorLV = new G4LogicalVolume(collimatorSolid,          // solid
						      material,                 // material
						      name + "_collimator_lv"); // name

  // set colour to dark green
  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  collimatorLV->SetVisAttributes(collimatorVisAttr);

#ifndef NOUSERLIMITS
  collimatorLV->SetUserLimits(BDSGlobalConstants::Instance()->GetDefaultUserLimits());
#endif

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(collimatorLV);
  RegisterSensitiveVolume(collimatorLV);

  new G4PVPlacement(0,                       // rotation
		    (G4ThreeVector)0,        // position
		    collimatorLV,            // its logical volume
		    name + "_collimator_pv", // its name
		    containerLogicalVolume,  // its mother  volume
		    false,		     // no boolean operation
		    0,		             // copy number  
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());

  if (buildVacuumAndAperture)
    {
      G4Material* vMaterial = BDSMaterials::Instance()->GetMaterial(vacuumMaterial);
      G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
						      vMaterial,            // material
						      name + "_vacuum_lv"); // name

      vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
      RegisterLogicalVolume(vacuumLV);
#ifndef NOUSERLIMITS
      vacuumLV->SetUserLimits(BDSGlobalConstants::Instance()->GetDefaultUserLimits());
#endif

      new G4PVPlacement(0,                       // rotation
			(G4ThreeVector)0,        // position
			vacuumLV,                // its logical volume
			name + "_vacuum_pv",     // its name
			containerLogicalVolume,  // its mother  volume
			false,		         // no boolean operation
			0,		         // copy number  
			BDSGlobalConstants::Instance()->GetCheckOverlaps());
    } 
}

BDSCollimatorBase::~BDSCollimatorBase()
{;}

