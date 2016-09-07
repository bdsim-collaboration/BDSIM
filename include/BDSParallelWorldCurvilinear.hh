#ifndef BDSPARALLELWORLDCURVILINEAR_H
#define BDSPARALLELWORLDCURVILINEAR_H

#include "G4VUserParallelWorld.hh"

class G4VisAttributes;

/**
 * @brief A parallel world for curvilinear coordinates and sensitivity.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldCurvilinear: public G4VUserParallelWorld
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
