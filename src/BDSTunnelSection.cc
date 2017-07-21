#include "BDSTunnelSection.hh"

#include "G4VSolid.hh"

BDSTunnelSection::BDSTunnelSection(G4String              nameIn,
				   G4double              chordLengthIn,
				   G4double              angleIn,
				   BDSGeometryComponent* tunnelGeometry,
				   G4VSolid*             innerIntersectionSolidIn):
  BDSAcceleratorComponent(nameIn, chordLengthIn, angleIn, "tunnel"),
  innerIntersectionSolid(innerIntersectionSolidIn)
{
  // chordLength is calculated from the angle and the arcLength by BDSAcceleratorComponent
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
