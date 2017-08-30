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
  G4Material* GetMaterial(G4String aMaterial)const;
  /// Get element by name
  G4Element*  GetElement(G4String aSymbol)const;

protected:
  BDSMaterials();
  /// Add G4Material
  void AddMaterial(G4Material* aMaterial,G4String aName);
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
  void AddMaterial(G4String aName, 
		   G4double density, 
		   G4State  state, 
		   G4double temp, 
		   G4double pressure,
		   std::list<G4String> components,
		   std::list<Type> componentsFractions);

  /// Check if material is defined
  G4bool CheckMaterial(G4String aMaterial)const;
  /// Check if element is defined
  G4bool CheckElement(G4String aSymbol)const;

  /// map of materials, convention name lowercase
  std::map<G4String,G4Material*> materials; 
  /// map of elements, convention name lowercase
  std::map<G4String,G4Element*>  elements; 
private:
  /// Singleton instance
  static BDSMaterials* _instance;

  ///@{ Methods called by constructor
  void DefineElements();
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
  void AddElement(G4Element* aElement,G4String aName);
  void AddElement(G4String aName, G4String aSymbol, G4double itsZ, G4double itsA);
  ///@}
  /// Material tables for storing pointers
  std::vector<G4MaterialPropertiesTable*> propertiesTables;
  /// Create new properties table and store in vector
  G4MaterialPropertiesTable* CreatePropertiesTable();
};

#endif
