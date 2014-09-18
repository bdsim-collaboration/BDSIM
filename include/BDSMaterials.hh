/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Added Get method to enable access to materials by passing a string
*/
#ifndef BDSMaterials_h
#define BDSMaterials_h 1

#include <map>
#include <list>

#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

class BDSMaterials
{
public:

  static BDSMaterials* Instance();
  ~BDSMaterials(); //SPM

  /// converts parser material list
  void PrepareRequiredMaterials();

  void AddMaterial(G4Material* aMaterial,G4String aName); //SPM
  void AddMaterial(G4String aName, G4double itsZ, G4double itsA, G4double itsDensity); //SPM

  template <typename Type> void AddMaterial(
			G4String aName, 
			G4double itsDensity, 
			G4State  itsState, 
			G4double itsTemp, 
			G4double itsPressure,
			std::list<const char*> itsComponents,
			std::list<Type> itsComponentsFractions);

  void AddElement(G4Element* aElement,G4String aName); //SPM
  void AddElement(G4String aName, G4String aSymbol, G4double itsZ, G4double itsA); //SPM

  static void ListMaterials();

  G4Material* GetMaterial(G4String aMaterial); //SPM
  G4Element*  GetElement(G4String aSymbol); //SPM
  G4Element* GetElement(const char* aSymbol);

  G4bool CheckMaterial(G4String aMaterial); 
  G4bool CheckElement(G4String aSymbol); 

protected:
  BDSMaterials();
  std::map<G4String,G4Material*> materials; //SPM
  std::map<G4String,G4Element*>  elements; //SPM
private:
  void Initialise(); //SPM

  static BDSMaterials* _instance;
  G4MaterialPropertiesTable* airMaterialPropertiesTable;
  G4MaterialPropertiesTable* celluloseMaterialPropertiesTable;
  G4MaterialPropertiesTable* fsMaterialPropertiesTable;
  G4MaterialPropertiesTable* petMaterialPropertiesTable;
  G4MaterialPropertiesTable* vacMaterialPropertiesTable;

};

#endif
