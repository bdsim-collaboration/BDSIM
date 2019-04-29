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
#ifndef BDSREGION_H
#define BDSREGSION_H

#include "globals.hh"

class G4ProductionCuts;

namespace GMAD
{
  class Region;
}

/**
 * @brief Range cuts for a region. Help with defaults.
 *
 * @author Laurie Nevay
 */

class BDSRegion
{
public:
  BDSRegion();
  BDSRegion(G4double defaultRangeCutIn,
	    G4double rangeCutElectornsIn,
	    G4double rangeCutPositronsIn,
	    G4double rangeCutProtonsIn,
	    G4double rangeCutPhotonsIn);
  BDSRegion(const BDSRegion& defaultValue,
	    G4double defaultRangeCutIn,
	    G4double rangeCutElectornsIn,
	    G4double rangeCutPositronsIn,
	    G4double rangeCutProtonsIn,
	    G4double rangeCutPhotonsIn);
  BDSRegion(const BDSRegion& defaultValue,
	    const GMAD::Region& regionIn);
	    
  ~BDSRegion();

  /// Produce a production cuts object (doesn't retain ownership).
  G4ProductionCuts* Cuts();

private:
  G4double defaultRangeCut;
  G4double rangeCutElectrons;
  G4double rangeCutPositrons;
  G4double rangeCutProtons;
  G4double rangeCutPhotons;
}

#endif
