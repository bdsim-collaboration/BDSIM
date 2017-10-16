#ifndef BDSPHYSICALCONSTANTS_H
#define BDSPHYSICALCONSTANTS_H

#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace BDS
{
  /// speed of light / 1 GeV, used for scaling in brho calculation
  static const G4double cOverGeV = CLHEP::c_light / CLHEP::GeV;
}


#endif
