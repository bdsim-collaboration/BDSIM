#ifndef BDSBEAMLINEPLACEMENTBUILDER_H
#define BDSBEAMLINEPLACEMENTBUILDER_H

#include "parser/placement.h"

#include <vector>

class BDSBeamline;

namespace BDS
{
  /// Construct a BDSBeamline instance with all external geometry. The
  /// geometry is constructed using the geometry factory (and owned by it)
  /// but the beam line is necessary for extent calculation for the world.
  BDSBeamline* BuildPlacementGeometry(const std::vector<GMAD::Placement>& placements);
}

#endif
