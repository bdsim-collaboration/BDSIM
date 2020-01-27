/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSLINKRUNMANAGER_H
#define BDSLINKRUNMANAGER_H

#include "G4RunManager.hh"

/**
 * @brief Wrapper from G4RunManager that provides more output.
 *
 */

class BDSLinkRunManager: public G4RunManager
{
public:
  BDSLinkRunManager(){;}
  virtual ~BDSLinkRunManager(){;}

  /// Run G4RunManager::Initialise() and update BDSPrimaryGeneratorAction with knowledge
  /// of the world extent for coordinate checking.
  virtual void Initialize();
};
#endif

