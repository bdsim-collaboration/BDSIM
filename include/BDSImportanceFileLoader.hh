/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef BDSIMPORTANCEFILELOADER_H
#define BDSIMPORTANCEFILELOADER_H

#include "globals.hh" // geant4 globals / types
#include <map>
#include <vector>

/**
 * @brief A loader for importance values used in importance sampling.
 *
 *
 * 
 * @author Will Shields
 */

class BDSImportanceFileLoader
{
public:
  /// Singleton accessor.
  static BDSImportanceFileLoader* Instance();

  ~BDSImportanceFileLoader();

  std::map<G4String, G4double> Load(G4String fileName);

  std::vector<G4String> volumes;

  std::vector<G4double> importanceValues;

private:
  /// Private default constructor as singleton
  BDSImportanceFileLoader();

  /// Singleton instance
  static BDSImportanceFileLoader* instance;

};

#endif
