#ifndef BDSPARALLELWORLDCURVILINEARBRIDGE_H
#define BDSPARALLELWORLDCURVILINEARBRIDGE_H

#include "BDSBeamlinePlacement.hh"

#include "G4VUserParallelWorld.hh"

class G4VisAttributes;

/**
 * @brief A parallel world for bridiging curvilinear volumes.
 *
 * Specifically, this curvilinear 
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldCurvilinearBridge: public G4VUserParallelWorld, public BDSBeamlinePlacement
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
