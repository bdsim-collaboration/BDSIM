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
#ifndef BDSSCORERVOLUMEFILTER_H
#define BDSSCORERVOLUMEFILTER_H

#include "globals.hh"
#include "G4VSDFilter.hh"

class G4LogicalVolume;

/**
 * @brief SD filter for a particular volume.
 * 
 * @author Robin Tesse
 */

class BDSScorerVolumeFilter: public G4VSDFilter
{
public:
  BDSScorerVolumeFilter(G4String name,
			G4LogicalVolume* referenceLVIn);
  
  virtual ~BDSScorerVolumeFilter();

  /// Whether the step will be accepted or rejected.
  virtual G4bool Accept(const G4Step* aStep) const;

private:
  G4LogicalVolume const* referenceLV;
};

#endif
