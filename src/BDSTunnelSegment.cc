#include "BDSTunnelSegment.hh"

#include <cmath>

class G4VSolid;

BDSTunnelSegment::BDSTunnelSegment(G4String              name,
				   G4double              arcLength,
				   G4double              angle,
				   BDSGeometryComponent* tunnelGeometry,
				   G4VSolid*             innerIntersectionSolidIn):
  BDSAcceleratorComponent(name, arcLength, angle, "tunnel"),
  innerIntersectionSolid(innerIntersectionSolidIn)
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
