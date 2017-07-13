#ifndef BDSPARALLELWORLDCURVILINEARBRIDGE_H
#define BDSPARALLELWORLDCURVILINEARBRIDGE_H

#include "G4VUserParallelWorld.hh"

class G4VisAttributes;

/**
 * @brief A parallel world for bridiging curvilinear volumes.
 *
 * Specifically, this curvilinear world is filled with small volumes
 * that cover the gaps in the curvilinear world. This provides a fall
 * back in case we find the world volume in a look up where we would
 * really like to find curvilinear coordinates.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldCurvilinearBridge: public G4VUserParallelWorld
{
public:
  BDSParallelWorldCurvilinearBridge();
  virtual ~BDSParallelWorldCurvilinearBridge();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

private:
  /// Visualisation attributes for the world volume
  G4VisAttributes* clbWorldVis;
};

#endif
