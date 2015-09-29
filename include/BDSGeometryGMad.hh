#ifndef BDSGEOMETRYGMAD
#define BDSGEOMETRYGMAD

#include "BDSGeometry.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include <fstream>

/**
 * @brief GMad geometry parser that dynamically creates Geant4 geometry.
 *
 **/

class BDSGeometryGMad: public BDSGeometry
{
public:
  BDSGeometryGMad(G4String);
  virtual void Construct(G4LogicalVolume *marker);

private:
  std::ifstream inputf;
  G4String GetWord();
  void GetParameter(G4double& x, G4String name, G4String lastToken);
  void GetParameter(G4String& lval, G4String name, G4String lastToken);
  void Error();
};

#endif
