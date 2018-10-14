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
#ifndef BDSCRYSTALREGISTRY_H
#define BDSCRYSTALREGISTRY_H

#include "G4Types.hh"

#include <set>

class G4LogicalVolume;

/**
 * @brief Set of logical volume pointers for crystal physics biasing.
 * 
 * @author Laurie Nevay
 */

class BDSCrystalRegistry
{
private:
  /// Typedefs up first so we can declare public iterators.
  typedef std::set<G4LogicalVolume*> CrystalSet;

  /// Storate of registerd information.
  CrystalSet crystals;
  
public:
  /// Accessor for registry.
  static BDSCrystalRegistry* Instance();

  ~BDSCrystalRegistry();

  /// Register a crystal.
  inline void RegisterCrystal(G4LogicalVolume* crystalIn) {crystals.insert(crystalIn);}

  ///@{ Iterator mechanics
  typedef CrystalSet::iterator       iterator;
  typedef CrystalSet::const_iterator const_iterator;
  iterator       begin()       {return crystals.begin();}
  iterator       end()         {return crystals.end();}
  const_iterator begin() const {return crystals.begin();}
  const_iterator end()   const {return crystals.end();}
  G4bool         empty() const {return crystals.empty();}
  ///@}

  /// Access all crystals.
  inline std::set<G4LogicalVolume*> GetCrystals() const {return crystals;}
  
private:
  /// Private constructor to enforce singleton pattern
  BDSCrystalRegistry();

  /// Singleton instance
  static BDSCrystalRegistry* instance;
};

#endif
