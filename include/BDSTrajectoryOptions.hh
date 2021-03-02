/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#ifndef BDSTRAJECTORYOPTIONS_H
#define BDSTRAJECTORYOPTIONS_H

#include "G4Types.hh"

/**
 * @brief Holder for trajectory storage options.
 * 
 * @author Laurie Nevay
 */

namespace BDS
{
  struct TrajectoryOptions
  {
    G4bool suppressTransportationSteps = false;
    G4bool storeKineticEnergy   = false;
    G4bool storeMomentumVector  = false;
    G4bool storeProcesses       = false;
    G4bool storeTime            = false;
    G4bool storeLocal           = false;
    G4bool storeLinks           = false;
    G4bool storeIon             = false;
  };
}
  
#endif
