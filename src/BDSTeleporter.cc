#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTeleporter.hh"

#include "globals.hh" //G4 global constants & types
#include "G4Box.hh" 
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"

#include <cmath>


BDSTeleporter::BDSTeleporter(const G4double length,
			     BDSFieldInfo*  vacuumFieldInfoIn):
  BDSAcceleratorComponent("teleporter", length, 0, "teleporter"),
  vacuumFieldInfo(vacuumFieldInfoIn)
{;}

void BDSTeleporter::Build()
{
  BDSAcceleratorComponent::Build(); // create container
  
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

G4ThreeVector BDS::CalculateTeleporterDelta(BDSBeamline* thebeamline)
{
  if (thebeamline->empty())
    {return G4ThreeVector();}
  // get position of last item in beamline
  // and then calculate necessary offset teleporter should apply
  G4ThreeVector lastitemposition  = thebeamline->back()->GetReferencePositionEnd();
  G4ThreeVector firstitemposition = thebeamline->front()->GetReferencePositionStart();
  G4ThreeVector delta             = lastitemposition - firstitemposition;
  
  G4cout << "Calculating Teleporter delta"   << G4endl;
  G4cout << "Last item end position:       " << lastitemposition  << " mm" << G4endl;
  G4cout << "First item start position:    " << firstitemposition << " mm" << G4endl;
  G4cout << "Teleport delta:               " << delta << " mm" << G4endl;

  return delta;
}

BDSTeleporter::~BDSTeleporter()
{
  delete vacuumFieldInfo;
}
