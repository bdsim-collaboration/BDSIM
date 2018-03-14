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
#ifndef BDSMAGNETSTRENGTH_H
#define BDSMAGNETSTRENGTH_H

#include "globals.hh" // geant4 globals / types

#include <map>
#include <ostream>
#include <vector>

/**
 * @brief Efficient storage of magnet strengths.
 *
 * Based on std::map, this class stores magnet strengths. 
 *
 * As there will be one of 
 * these for every magnet and they may in future have to go up to high order 
 * normal plus skew (~40 doubles), it is more efficient to only store the required
 * parameters. A pure quadrupole has no need of k20s, but a multipole may do. A static
 * vector contains the possible magnet strength parameters that can be checked against.
 * If a parameter is not specified in the map, its value is return as 0.
 *
 * Angle in rad, Field in Geant4 units. k strengths as original (ie not converted to G4).
 * 
 * @author Laurie Nevay
 */

class BDSMagnetStrength
{
private:
  /// Typedefs up first so we can declare public iterators.
  typedef std::map<G4String, G4double> StrengthMap;

  /// A map containing the values for various magnet strengths. It only contains
  /// the set magnet strengths. If a strength isn't specified, it's assumed to be zero
  /// without having to store the 0 value.
  StrengthMap strengths;
  
public:
  /// Default constructor does nothing as class will return 0 for uninitialised keys
  BDSMagnetStrength(){;}
  /// This constructor allows instantiation with a map of keys and values.
  explicit BDSMagnetStrength(std::map<G4String, G4double> keyvalues);

  ~BDSMagnetStrength(){;}

  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSMagnetStrength const &st);

  /// Accessors with array / map [] operator
  G4double& operator[](const G4String key);
  const G4double& operator[](G4String key) const;
  const G4double& at(G4String key) const {return (*this)[key];}

  static const std::vector<G4String> AllKeys() {return keys;}

  /// Accessor for normal component keys - k1 - k12
  inline std::vector<G4String> NormalComponentKeys() const
  {return normalComponentKeys;}

  /// Accessor for skew component keys - k1 - k12
  inline std::vector<G4String> SkewComponentKeys() const
  {return skewComponentKeys;}

  /// Accessor for all normal components - k1 - k12
  std::vector<G4double> NormalComponents() const;

  /// Accessor for all skew components - k1 - k12
  std::vector<G4double> SkewComponents() const;
  
  ///@{ iterator mechanics
  typedef StrengthMap::iterator       iterator;
  typedef StrengthMap::const_iterator const_iterator;
  iterator       begin()       {return strengths.begin();}
  iterator       end()         {return strengths.end();}
  const_iterator begin() const {return strengths.begin();}
  const_iterator end()   const {return strengths.end();}
  G4bool         empty() const {return strengths.empty();}
  ///@}
  
private:
  /// Whether or not the supplied key is a valid magnet strength parameter
  G4bool ValidKey(const G4String key) const;

  /// Accessor similar to [] but without linear search through keys to check validity
  /// for fast internal use.
  const G4double& GetValue(const G4String key) const;

  /// Vector of the allowed strength parameters
  static const std::vector<G4String> keys;

  /// Vector of the normal component strength parameters
  static const std::vector<G4String> normalComponentKeys;

  /// Vector of the normal component strength parameters
  static const std::vector<G4String> skewComponentKeys;

  /// Keep a single copy of 0.0 as it needs to be returned as a reference not a value
  static const G4double zero;

  /// Dummy variable that can be overwritten
  static G4double variable;
};


#endif
