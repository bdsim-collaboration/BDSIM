#include "BDSReadOutGeometry.hh"

#include "BDSAcceleratorModel.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VReadOutGeometry.hh"
#include "G4VSolid.hh"

BDSReadOutGeometry::BDSReadOutGeometry(G4String name):
  G4VReadOutGeometry(name)
{;}

BDSReadOutGeometry::~BDSReadOutGeometry()
{;}

G4VPhysicalVolume* BDSReadOutGeometry::Build()
{
  return BDSAcceleratorModel::Instance()->GetReadOutWorldPV();
}


G4LogicalVolume* BDS::BuildReadOutVolume(G4String name,
					 G4double chordLength,
					 G4double angle)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (chordLength < 1e-15) return NULL;

  G4double roRadius      = BDSGlobalConstants::Instance()->GetSamplerDiameter()*0.5;
  G4Material* roMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  G4VSolid* roSolid      = NULL;
  if (angle < 1e-15)
    {
      //angle is zero - build a box
      roSolid = new G4Box(name + "_ro_solid", // name
			  roRadius,           // x half width
			  roRadius,           // y half width
			  chordLength*0.5);     // z half width
    }
  else
    {
      // angle is finite!
      G4int orientation = BDS::CalculateOrientation(angle);
      G4double in_z     = cos(0.5*fabs(angle*0.5)); 
      G4double in_x     = sin(0.5*fabs(angle*0.5));
      G4ThreeVector inputface  = G4ThreeVector(-orientation*in_x, 0.0, -1.0*in_z);
      //-1 as pointing down in z for normal
      G4ThreeVector outputface = G4ThreeVector(-orientation*in_x, 0.0, in_z);

      roSolid = new G4CutTubs(name + "_ro_solid", // name
			      0,                  // inner radius
			      roRadius,           // outer radius
			      chordLength*0.5,         // half length (z)
			      0,                  // rotation start angle
			      CLHEP::twopi,       // rotation sweep angle
			      inputface,          // input face normal vector
			      outputface);        // output face normal vector
    }

  // note material not strictly necessary in geant4 > v10, but required for
  // v9 even though not used and doesn't affect simulation - leave for compatability
  G4LogicalVolume* readOutLV =  new G4LogicalVolume(roSolid,          // solid
						    roMaterial,       // material
						    name + "_ro_lv"); // name

  return readOutLV;
}
