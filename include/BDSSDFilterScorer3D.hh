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
#ifndef BDSSCORERFILTER_H
#define BDSSCORERFILTER_H

#include "globals.hh"
#include "G4VSDFilter.hh"

class BDSScorerInfo;
class BDSSDFilterTime;
class BDSSDFilterLogicalVolume;
class BDSSDFilterMaterial;
class G4SDParticleWithEnergyFilter;

/**
 * @brief SD filter for time coordinate, energy, particle type and volume.
 * 
 * @author Robin Tesse
 */

class BDSSDFilterScorer3D: public G4VSDFilter
{
public:
  BDSSDFilterScorer3D(G4String name,
		  const BDSScorerInfo* info);
  
  virtual ~BDSSDFilterScorer3D();

  /// Whether to accept or reject a step.
  virtual G4bool Accept(const G4Step* aStep) const;
  
private:
  /// No default constructor required.
  BDSSDFilterScorer3D() = delete;
  
  G4SDParticleWithEnergyFilter* particleWithKineticEnergyFilter;
  BDSSDFilterTime*              timeFilter;
  BDSSDFilterLogicalVolume*     volumeFilter;
  BDSSDFilterMaterial*          materialFilter;
};

#endif
