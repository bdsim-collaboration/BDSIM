#include "BDSGeometryComponent.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals


BDSSimpleComponent::BDSSimpleComponent(G4String              name,
				       BDSGeometryComponent* component,
                                       G4double              arcLength,
				       G4double              angle,
				       G4ThreeVector         inputFaceNormal,
				       G4ThreeVector         outputFaceNormal,
				       BDSBeamPipeInfo*      beamPipeInfo):
  BDSAcceleratorComponent(name, arcLength, angle, "simpleComponent", beamPipeInfo,
			  inputFaceNormal, outputFaceNormal)
{
  RegisterDaughter(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
}

BDSSimpleComponent::BDSSimpleComponent(G4String              name,
				       G4double              arcLength,
				       G4double              angle,
				       G4VSolid*             containerSolidIn,
				       G4LogicalVolume*      containerLogicalVolumeIn,
				       G4ThreeVector         inputFaceNormal,
				       G4ThreeVector         outputFaceNormal,
				       BDSBeamPipeInfo* beamPipeInfo):
  BDSAcceleratorComponent(name, arcLength, angle, "simpleComponent", beamPipeInfo,
			  inputFaceNormal, outputFaceNormal)
{
  containerSolid         = containerSolidIn;
  containerLogicalVolume = containerLogicalVolumeIn;
}
