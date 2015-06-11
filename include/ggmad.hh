// gmad geometry and field map driver for BDSIM

#ifndef _GGMAD_H
#define _GGMAD_H


#include "globals.hh"
#include "G4LogicalVolume.hh"

#include <fstream>
#include "BDSGeometry.hh"

class GGmadDriver : public BDSGeometry{

public:
  GGmadDriver(G4String);
  virtual void Construct(G4LogicalVolume *marker);

private:
  std::ifstream inputf;
  G4String getWord();
  void getParameter(G4double& x, G4String name, G4String lastToken);
  void getParameter(G4String& lval, G4String name, G4String lastToken);
  void error() {G4cout<<"ggmad parser: error in file format"<<G4endl;}
};

//void GetMaterial(G4Material *&theMaterial, G4String material);

#endif
