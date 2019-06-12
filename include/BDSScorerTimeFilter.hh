/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BDSSCORERTIMEFILTER_H
#define BDSSCORERTIMEFILTER_H

#include "globals.hh"
#include "G4VSDFilter.hh"

#include "CLHEP/Units/SystemOfUnits.h"

/**
 * @brief Filter for time value for a sensitive detector.
 *
 * @author Robin Tesse
 */

class BDSScorerTimeFilter: public G4VSDFilter
{
public:
  /// Constructor with default values.
  BDSScorerTimeFilter(G4String name,
		      G4double lowerLimitIn = 0.0,
		      G4double upperLimitIn = 1*CLHEP::s);
  
  virtual ~BDSScorerTimeFilter(){;}

  /// Whether a step will be accepted or rejected.
  virtual G4bool Accept(const G4Step*) const;
  
private:
  /// No default constructor required.
  BDSScorerTimeFilter() = delete;
  G4double lowerLimit;
  G4double upperLimit;
};

#endif
