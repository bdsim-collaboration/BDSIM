#include "BDSGeometryComponent.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals


BDSSimpleComponent::BDSSimpleComponent(G4String              name,
				       BDSGeometryComponent* component,
                                       G4double length,
				       G4double              angle):
  BDSAcceleratorComponent(name, length, angle, "simpleComponent")
{
  InheritObjects(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
}
