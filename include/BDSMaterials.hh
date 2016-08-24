#ifndef BDSMATERIALS_H
#define BDSMATERIALS_H

#include <list>
#include <map>
#include <string>

#include "globals.hh"
#include "G4Material.hh"

/**
 * @brief A class for all material definitions known to BDSIM.
 * Additional materials can be added in the parser as well.
 */
class BDSMaterials
{
public:

  static BDSMaterials* Instance();
  ~BDSMaterials(); 

  /// converts parser material list
  void PrepareRequiredMaterials(G4bool verbose = false);

  /** Add materials
      @param[in] Z        atomic number
      @param[in] A        mole mass in g/mole
      @param[in] density  in g/cm3
      @param[in] state    solid/gas
      @param[in] temp     in kelvin
      @param[in] pressure in atm
  */ 
  void AddMaterial(G4Material* aMaterial,G4String aName);
  void AddMaterial(G4String aName,
		   G4double Z,
		   G4double A,
		   G4double density,
		   G4State  state, 
		   G4double temp, 
		   G4double pressure);

  /** Add materials
      @param[in] density  in g/cm3
      @param[in] state    solid/gas
      @param[in] temp     in kelvin
      @param[in] pressure in atm
  */
  template <typename Type> void AddMaterial(
			G4String aName, 
			G4double density, 
			G4State  state, 
			G4double temp, 
			G4double pressure,
			std::list<std::string> components,
			std::list<Type> componentsFractions);

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
  ///@{ Material tables
  G4MaterialPropertiesTable* airMaterialPropertiesTable       = nullptr;
  G4MaterialPropertiesTable* celluloseMaterialPropertiesTable = nullptr;
  G4MaterialPropertiesTable* fsMaterialPropertiesTable        = nullptr;
  G4MaterialPropertiesTable* nbk7MaterialPropertiesTable      = nullptr;
  G4MaterialPropertiesTable* mptLanex                         = nullptr;
  G4MaterialPropertiesTable* mptLanex2                        = nullptr;
  G4MaterialPropertiesTable* mptGOSLanex                      = nullptr;
  G4MaterialPropertiesTable* mptGOSLanexRi1                   = nullptr;
  G4MaterialPropertiesTable* mptMedex                         = nullptr;
  G4MaterialPropertiesTable* mptPETLanex                      = nullptr;
  G4MaterialPropertiesTable* mpt_YAG                          = nullptr;
  G4MaterialPropertiesTable* petMaterialPropertiesTable       = nullptr;
  G4MaterialPropertiesTable* ups923a_mt                       = nullptr;
  G4MaterialPropertiesTable* vacMaterialPropertiesTable       = nullptr;
  ///@}
};

#endif
