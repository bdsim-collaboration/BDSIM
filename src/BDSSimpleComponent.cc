#include "BDSGeometryComponent.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals


BDSSimpleComponent::BDSSimpleComponent(G4String              nameIn,
				       BDSGeometryComponent* component,
                                       G4double              arcLengthIn,
				       G4double              angleIn,
				       G4ThreeVector         inputFaceNormalIn,
				       G4ThreeVector         outputFaceNormalIn,
				       BDSBeamPipeInfo*      beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, "simpleComponent", beamPipeInfoIn,
			  inputFaceNormalIn, outputFaceNormalIn)
{
  RegisterDaughter(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
}

BDSSimpleComponent::BDSSimpleComponent(G4String              nameIn,
				       G4double              arcLengthIn,
				       G4double              angleIn,
				       G4VSolid*             containerSolidIn,
				       G4LogicalVolume*      containerLogicalVolumeIn,
				       G4ThreeVector         inputFaceNormalIn,
				       G4ThreeVector         outputFaceNormalIn,
				       BDSBeamPipeInfo*      beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, "simpleComponent", beamPipeInfoIn,
			  inputFaceNormalIn, outputFaceNormalIn)
{
  containerSolid         = containerSolidIn;
  containerLogicalVolume = containerLogicalVolumeIn;
}
