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
#ifndef BDSSCORERINFO_H
#define BDSSCORERINFO_H

#include "BDSScorerType.hh"

#include "globals.hh"         // geant4 types / globals

namespace GMAD
{
  class Scorer;
}
class G4ParticleDefinition;

/**
 * @brief Recipe class for scorer. Checks values.
 *
 * Convert a parser scorer instance to one using Geant4 types
 * and units. Check values and find particle definition.
 *
 * @author Robin Tesse
 */

class BDSScorerInfo
{
public:
  /// Constructor. If upgradeTo3D is true, some scorer types will
  /// be upgraded to 3d as required for use in a mesh.
  explicit BDSScorerInfo(const GMAD::Scorer& scorer,
			 G4bool upgradeTo3D = false);
  
  BDSScorerType scorerType;          ///< Scorer type.
  G4String      name;                ///< Scorer name.
  G4ParticleDefinition* particle;    ///< Particle filter
  G4double      minimumEnergy;       ///< Minimal energy
  G4double      maximumEnergy;       ///< Maximal energy
  G4double      minimumTime;         ///< Minimum time
  G4double      maximumTime;         ///< Maximum time
  G4String      filename;            ///< Name of the conversion factor file
  
private:
  /// Pivate default constructor as unused.
  BDSScorerInfo();
};

#endif
