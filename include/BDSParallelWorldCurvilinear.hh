#ifndef BDSPARALLELWORLDCURVILINEAR_H
#define BDSPARALLELWORLDCURVILINEAR_H

#include "BDSBeamlinePlacement.hh"

#include "G4VUserParallelWorld.hh"

class G4VisAttributes;

/**
 * @brief A parallel world for curvilinear coordinates.
 *
 * The world isn't used in the main (bdsim.cc) with a paralle world physics
 * process so the boundaries in this world don't affect tracking.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldCurvilinear: public G4VUserParallelWorld, public BDSBeamlinePlacement
{
public:
  BDSParallelWorldCurvilinear();
  virtual ~BDSParallelWorldCurvilinear();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

private:
  /// Visualisation attributes for the world volume
  G4VisAttributes* clWorldVis;
};

#endif
