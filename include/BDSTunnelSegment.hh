#ifndef BDSTUNNELSEGMENT_H
#define BDSTUNNELSEGMENT_H

#include "BDSAcceleratorComponent.hh"
#include "globals.hh" // geant4 types / globals

class BDSGeometryComponent;
class G4VSolid;

/**
 * @brief Class that represents a section of tunnel. Tunnel segments are
 * generally simple BDSGeometryComponent instances, but holding them in a
 * beamline instance as well as placing them requires that they really be
 * a type of (inherit) BDSAcceleratorComponent. This adds the minimal useful
 * information requried for using a beamline and for placement (independent of
 * being required for being stored in a beamline).
 *
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTunnelSegment: public BDSAcceleratorComponent
{
public:
  /// Constructor takes BDSGeometryComponent instance of already
  /// constructed geometry. The inner intersection solid is a solid that
  /// represents the cavity inside the tunnel. It can be used to intersect
  /// with any geometry such as the supports to ensure they always fit without
  /// having detailed knowledge about the floor and tunnel geometry itself.
  BDSTunnelSegment(G4String              name,
		   G4double              chordLength,
		   G4double              angle,
		   BDSGeometryComponent* tunnelGeometry,
		   G4VSolid*             innerIntersectionSolidIn);

  ~BDSTunnelSegment(){;}

  /// Override BDSAcceleratorComponent::Initialise() method to avoid doing
  /// anything. Doing this avoids wrong and useless read out volume being created.
  /// Note, this will mean that the private internal variable 'initialised' will always
  /// be false even though it effectively 'is' initialised.
  virtual void Initialise();

  /// Have to provide implementation - does nothing
  virtual void BuildContainerLogicalVolume();

  G4VSolid* innerIntersectionSolid;
};

#endif
