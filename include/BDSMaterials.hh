#ifndef BDSMATERIALS_H
#define BDSMATERIALS_H

#include <map>
#include <list>

#include "globals.hh"
#include "G4Material.hh"

class G4MaterialPropertiesTable;

class BDSMaterials
{
public:

  static BDSMaterials* Instance();
  ~BDSMaterials(); 

  /// converts parser material list
  void PrepareRequiredMaterials();

  ///@{
  /** Add materials
      @param[in] Z        atomic number
      @param[in] A        mole mass in g/mole
      @param[in] density  in g/cm3
      @param[in] state    solid/gas
      @param[in] temp     in kelvin
      @param[in] pressure in atm
  */ 
  void AddMaterial(G4Material* aMaterial, G4String aName);
  void AddMaterial(G4String aName,
		   G4double Z,
		   G4double A,
		   G4double density,
		   G4State  state, 
		   G4double temp, 
		   G4double pressure);

  template <typename Type> void AddMaterial(
			G4String aName, 
			G4double density, 
			G4State  state, 
			G4double temp, 
			G4double pressure,
			std::list<std::string> components,
			std::list<Type> componentsFractions);
  ///@}

  void AddElement(G4Element* aElement,G4String aName);
  void AddElement(G4String aName, G4String aSymbol, G4double itsZ, G4double itsA);
  
  /// output available materials
  // static since BDSMaterials construction needs BDSGlobalConstants, which needs full options definitions (not ideal, but alas)
  static void ListMaterials();

  G4Material* GetMaterial(G4String aMaterial); 
  G4Element*  GetElement(G4String aSymbol); 

  G4bool CheckMaterial(G4String aMaterial); 
  G4bool CheckElement(G4String aSymbol); 

protected:
  BDSMaterials();
  // map of materials, convention name lowercase
  std::map<G4String,G4Material*> materials; 
  std::map<G4String,G4Element*>  elements; 
private:
  void Initialise(); 

  static BDSMaterials* _instance;
  G4MaterialPropertiesTable* airMaterialPropertiesTable;
  G4MaterialPropertiesTable* celluloseMaterialPropertiesTable;
  G4MaterialPropertiesTable* fsMaterialPropertiesTable;
  G4MaterialPropertiesTable* petMaterialPropertiesTable;
  G4MaterialPropertiesTable* vacMaterialPropertiesTable;

};

#endif
