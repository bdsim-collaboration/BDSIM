#ifndef BDSTELEPORTER_H
#define BDSTELEPORTER_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSBeamline;
class BDSFieldObjects;

/**
 * @brief An element that unnaturally shifts the beam across its length - a fudge volume.
 * 
 * In some circular lattices, the input lattice doesn't match perfectly at the end, typically
 * due to numerical rounding issues accumulated over thousands of components. This is not
 * because of the geometry construction, but purely in the input MADX or gmad description.
 * Normal tracking programs don't show this offset as they really track through a sequence
 * of maps or functions and just apply them in a loop - it is defacto periodic. However, 
 * with real 3D geometry in global cartesian coordinates, we must artificially make the
 * lattice periodic. Enter the teleporter. This transports particles across its length
 * while maintaining the exact same transverse coordinates, which is normally unphysical.
 * 
 * @author Laurie Nevay
 */

class BDSTeleporter: public BDSAcceleratorComponent
{
public:
  BDSTeleporter(const G4double length,
		BDSFieldInfo*  vacuumFieldInfoIn);
  virtual ~BDSTeleporter();

private:
  /// Overridden method from BDSAcceleratorComponent that dictates the construction.
  virtual void Build();

  /// Overridden method from BDSAcceleratorComponent that creates the container volume.
  /// This is the only piece of geometry for the teleporter.
  virtual void BuildContainerLogicalVolume();

  /// Recipe for teleporter 'field'.
  BDSFieldInfo* vacuumFieldInfo;
};

namespace BDS
{
  G4ThreeVector CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline);
}

#endif
