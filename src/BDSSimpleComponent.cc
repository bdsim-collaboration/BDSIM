#include "BDSGeometryComponent.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals


BDSSimpleComponent::BDSSimpleComponent(G4String              name,
				       BDSGeometryComponent* component,
				       G4double              angle):
  BDSAcceleratorComponent(name, component->GetLengthZ(), angle, "simpleComponent")
{
  InheritObjects(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
}
