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
#ifndef BDSMATERIALS_H
#define BDSMATERIALS_H

#include <list>
#include <map>
#include <vector>

#include "globals.hh"
#include "G4Material.hh" // for G4State

class G4Element;
class G4MaterialPropertiesTable;

/**
 * @brief A class for all material definitions known to BDSIM.
 * Additional materials can be added in the parser as well.
 */

class BDSMaterials
{
public:

  /// Singleton pattern access
  static BDSMaterials* Instance();
  ~BDSMaterials(); 

  /// converts parser material list
  void PrepareRequiredMaterials(G4bool verbose = false);

  /// output available materials
  void ListMaterials()const;

  /// Get material by name
  G4Material* GetMaterial(G4String material) const;
  /// Get element by name
  G4Element*  GetElement(G4String symbol) const;

protected:
  BDSMaterials();
  /// Add G4Material
  void AddMaterial(G4Material* aMaterial, G4String aName);
  /// Add alias to a material
  void AddMaterial(G4String    aMaterial, G4String aName);
  /** Add materials
      @param[in] Z        atomic number
      @param[in] A        mole mass in g/mole
      @param[in] density  in g/cm3
      @param[in] state    solid/gas
      @param[in] temp     in kelvin
      @param[in] pressure in atm
  */
  void AddMaterial(G4String aName,
		   G4double Z,
		   G4double A,
		   G4double density,
		   G4State  state, 
		   G4double temp, 
		   G4double pressure);

  /** Add materials
      @param[in] density    in g/cm3
      @param[in] state      solid/gas
      @param[in] temp       in kelvin
      @param[in] pressure   in atm
      @param[in] components list of elements
      @param[in] componentsFractions list of fractions or integers of the elements
  */
  template <typename Type>
  void AddMaterial(G4String name, 
		   G4double density, 
		   G4State  state, 
		   G4double temperature, 
		   G4double pressure,
		   const std::list<G4String>& components,
		   const std::list<Type>&     componentFractions);

  /// Return element if defined (nullptr if not)
  G4Element* CheckElement(G4String symbol) const;

private:
  /// Singleton instance
  static BDSMaterials* _instance;

  ///@{ Methods called by constructor
  void DefineMetals();
  void DefineSuperconductors();
  void DefineNonMetalSolids();
  void DefineScintillators();
  void DefineLHCComponents();
  void DefineLiquids();
  void DefineGases();
  void DefinePlasmas();
  void DefineVacuums();
  ///@}

  ///@{ Add a G4Element
  void AddElement(G4Element* element, const G4String& symbol);
  void AddElement(G4String name, const G4String& symbol, G4double itsZ, G4double itsA);
  ///@}

  /// map of materials, convention name lowercase
  std::map<G4String,G4Material*> materials;
  /// map of elements, no lowercase convention
  std::map<G4String,G4Element*>  elements;
  /// Material tables for storing pointers
  std::vector<G4MaterialPropertiesTable*> propertiesTables;
  /// Create new properties table and store in vector
  G4MaterialPropertiesTable* CreatePropertiesTable();
};

#endif
