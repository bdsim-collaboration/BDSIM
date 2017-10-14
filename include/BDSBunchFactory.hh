#ifndef BDSBUNCHFACTORY_H
#define BDSBUNCHFACTORY_H

#include "BDSBunchType.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

class BDSBunch;

namespace GMAD
{
  class Beam;
}

/**
 * @brief Factory class for BDSBunch.
 * 
 * Instantiate the specific type of bunch distribution (class),
 * get the corresponding parameters from the gmad parser info
 * and attach to the initialised random number generator.
 * 
 * @author Jochem Snuverink
 */

class BDSBunchFactory
{
public:
  /// factory method
  static BDSBunch* CreateBunch(const GMAD::Beam& beam,
			       G4Transform3D beamlineTransform = G4Transform3D::Identity);

  static BDSBunch* CreateBunch(BDSBunchType      distrType,
			       const GMAD::Beam& beam,
			       G4Transform3D beamlineTransform = G4Transform3D::Identity);
};

#endif
