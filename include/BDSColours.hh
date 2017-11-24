/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSCOLOURS_H
#define BDSCOLOURS_H

#include "globals.hh"         // geant4 types / globals

#include <map>

class G4Colour;

/**
 * @brief Colour class that holds all colours used in BDSIM
 */

class BDSColours
{
public:
  static BDSColours* Instance(); ///< singleton pattern

  ~BDSColours();

  /// Get colour from name
  G4Colour* GetColour(G4String type);
  /// Get colour from magnet order (1=dipole, 2=quadrupole, etc.)
  G4Colour* GetMagnetColour(G4int magnetOrder) const;

private:
  BDSColours(); ///< Private constructor as singleton
  static BDSColours* _instance;

  /// A map of the magnet name for which order it is, which
  /// can be used to access the appropriate colour map
  std::map<G4int, G4String> magnetName;

  /// A map of the colour for each type or component by name
  std::map<G4String, G4Colour*> colours;
};

#endif
