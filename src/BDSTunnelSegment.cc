#include "BDSTunnelSegment.hh"

#include <cmath>

BDSTunnelSegment::BDSTunnelSegment(G4String              name,
				   G4double              arcLength,
				   G4double              angle,
				   BDSGeometryComponent* tunnelGeometry):
  BDSAcceleratorComponent(name, arcLength, angle, "tunnel")
{
  InheritObjects(tunnelGeometry);
  containerSolid         = tunnelGeometry->GetContainerSolid();
  containerLogicalVolume = tunnelGeometry->GetContainerLogicalVolume();
  extentX                = tunnelGeometry->GetExtentX();
  extentY                = tunnelGeometry->GetExtentY();
  extentZ                = tunnelGeometry->GetExtentZ();
}

void BDSTunnelSegment::Initialise()
{;}

void BDSTunnelSegment::BuildContainerLogicalVolume()
{;}
