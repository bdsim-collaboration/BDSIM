#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSDebug.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTeleporter.hh"

#include "globals.hh" //G4 global constants & types
#include "G4Box.hh" 
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"

#include <cmath>


BDSTeleporter::BDSTeleporter(const G4String      name,
			     const G4double      length,
			     const G4ThreeVector teleporterDeltaIn):
  BDSAcceleratorComponent(name, length, 0, "teleporter"),
  vacuumField(nullptr),
  teleporterDelta(teleporterDeltaIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " Constructing Teleporter of length: " 
	 << length/CLHEP::m << " m" << G4endl;
#endif
}

void BDSTeleporter::Build()
{
  BDSAcceleratorComponent::Build(); // create container

  // TBC
  BDSFieldInfo* vacuumFieldInfo = nullptr;
  
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
							    containerLogicalVolume,
							    true);
}

void BDSTeleporter::BuildContainerLogicalVolume()
{
  G4double radius = BDSGlobalConstants::Instance()->SamplerDiameter() * 0.5;
  containerSolid = new G4Box(name+"_container_solid",
			     radius,
			     radius,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");

  // register extents with BDSGeometryComponent base class
  SetExtent(BDSExtent(radius, radius, chordLength*0.5));
}

G4ThreeVector BDS::CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline)
{
  // get position of last item in beamline
  // and then calculate necessary offset teleporter should apply
  G4ThreeVector lastitemposition   = thebeamline->back()->GetReferencePositionEnd();
  G4ThreeVector firstitemposition  = thebeamline->front()->GetReferencePositionStart();
  G4ThreeVector delta              = lastitemposition - firstitemposition;
  
  G4cout << "Calculating Teleporter delta" << G4endl;
  G4cout << "Last item end position:       " << lastitemposition  << " mm" << G4endl;
  G4cout << "First item start position:    " << firstitemposition << " mm" << G4endl;
  G4cout << "Teleport delta:               " << delta << " mm" << G4endl;
  BDSGlobalConstants::Instance()->SetTeleporterDelta(delta);
  
  // calculate length of teleporter
  // beamline is built along z and sbend deflects in x
  // setting length here ensures that length is always the z difference
  G4double teleporterLength       = fabs(delta.z());

  // ensure there's no overlaps by reducing teleporter length by a few microns
  // it's ok to adjust the teleporter length and not the delta used by the teleporter stepper
  // as the stepper only uses 'h' the step length and not the delta.z component
  if (teleporterLength > 4*CLHEP::um)
    {teleporterLength -= 3*CLHEP::um;}
  G4cout << "Calculated teleporter length: " << teleporterLength << " mm" << G4endl;
  BDSGlobalConstants::Instance()->SetTeleporterLength(teleporterLength);

  return delta;
}

BDSTeleporter::~BDSTeleporter()
{
  delete vacuumField;
}
