#ifndef BDSBEAMLINEENDPIECEBUILDER_H
#define BDSBEAMLINEENDPIECEBUILDER_H

namespace BDS
{
  /// Loop over the flat beam line in BDSAcceleratorModel and inspect each
  /// element seeing if it has end pieces to be placed and if they will fit
  /// without overlapping.  If they will, append them to a separate beam line
  /// of end pieces that can be placed in the world volume.
  void BuildEndPieceBeamline();
}

#endif
