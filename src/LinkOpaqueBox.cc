#include "LinkOpaqueBox.hh"

LinkOpaqueBox::LinkOpaqueBox(BDSAcceleratorComponent* componentIn, G4int indexIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(componentIn),
  index(indexIn)
{
  // build container geometry.
  // One box minus smaller box to make room for collimator.

  // Make a container of vacuum

  // place component in box.
  // place sampler / output plane in box with index.

  // Update containerSolid and containerLV of BDSGeometryComponent.
  // Update extents of BDSGeometryComponent.
  // register daughters BDSGeometryComponent.


}


BDSExtent LinkOpaqueBox::GetExtent()
{
  return component->GetExtent();
}
