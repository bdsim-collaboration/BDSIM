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


class BDSMaterials
{
public:

 
  BDSMaterials();
  ~BDSMaterials(); //SPM

  void Initialise(); //SPM

  void AddMaterial(G4Material* aMaterial,G4String aName); //SPM
  void AddMaterial(G4String aName, G4double itsZ, G4double itsA, G4double itsDensity); //SPM

  void AddMaterial(	G4String aName, 
			G4double itsDensity, 
			G4State  itsState, 
			G4double itsTemp, 
			G4double itsPressure,
			std::list<char*> itsComponents,
			std::list<G4double> itsComponentsFractions); //SPM

  void AddMaterial(     G4String aName, 
                        G4double itsDensity, 
                        G4State  itsState, 
                        G4double itsTemp,
                        G4double itsPressure,
                        std::list<char*> itsComponents,
                        std::list<G4int> itsComponentsWeights); //SPM

  void AddElement(G4Element* aElement,G4String aName); //SPM
  void AddElement(G4String aName, G4String aSymbol, G4double itsZ, G4double itsA); //SPM

  static void ListMaterials();

  G4Material* GetMaterial(G4String aMaterial); //SPM
  G4Element*  GetElement(G4String aSymbol); //SPM

protected:
  std::map<G4String,G4Material*> materials; //SPM
  std::map<G4String,G4Element*>  elements; //SPM
private:


};

#endif
