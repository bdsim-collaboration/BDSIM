#include "BDSTunnelSection.hh"

BDSTunnelSection::BDSTunnelSection(G4String              name,
				   G4double              chordLengthIn,
				   G4double              angle,
				   BDSGeometryComponent* tunnelGeometry,
				   G4VSolid*             innerIntersectionSolidIn):
  BDSAcceleratorComponent(name, chordLengthIn, angle, "tunnel"),
  innerIntersectionSolid(innerIntersectionSolidIn)
{
  // chordLength is calcualted from the angle and the arcLength by BDSAcceleratorComponent
  // but here we only have chord length.  Just reassign chord length.
  chordLength = chordLengthIn;

  // copy special bits from BDSGeometryComponent.  Can't use BDSGeometryComponent in
  // initialiser list as c++ only allows you to use constructors one up in the inheritance.
  InheritObjects(tunnelGeometry);
  InheritExtents(tunnelGeometry);
  containerSolid         = tunnelGeometry->GetContainerSolid();
  containerLogicalVolume = tunnelGeometry->GetContainerLogicalVolume();
}

BDSTunnelSection::~BDSTunnelSection()
{
  delete innerIntersectionSolid;
}

void BDSTunnelSection::Initialise()
{;}

void BDSTunnelSection::BuildContainerLogicalVolume()
{;}
