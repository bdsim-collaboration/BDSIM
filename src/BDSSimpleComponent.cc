#include "BDSGeometryComponent.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals


BDSSimpleComponent::BDSSimpleComponent(G4String              name,
				       BDSGeometryComponent* component,
                                       G4double              length,
				       G4double              angle,
				       G4ThreeVector         inputFaceNormal,
				       G4ThreeVector         outputFaceNormal):
  BDSAcceleratorComponent(name, length, angle, "simpleComponent", nullptr,
			  inputFaceNormal, outputFaceNormal)
{
  RegisterDaughter(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
}

BDSSimpleComponent::BDSSimpleComponent(G4String              name,
				       G4double              length,
				       G4double              angle,
				       G4VSolid*             containerSolidIn,
				       G4LogicalVolume*      containerLogicalVolumeIn,
				       G4ThreeVector         inputFaceNormal,
				       G4ThreeVector         outputFaceNormal):
  BDSAcceleratorComponent(name, length, angle, "simpleComponent", false, nullptr,
			  inputFaceNormal, outputFaceNormal)
{
  containerSolid         = containerSolidIn;
  containerLogicalVolume = containerLogicalVolumeIn;
}
