#ifndef BDSBEAMPIPEINFO
#define BDSBEAMPIPEINFO

#include "BDSBeamPipeType.hh"
#include "globals.hh"         // geant4 types / globals
#include "G4Material.hh"

/**
 * @brief Holder struct of all information required to
 * create a beam pipe. This reduces the number of argument
 * to all magnet constructors plus aggregates common tasks
 * in the component factory.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct beamPipeInfo {
  BDSBeamPipeType beamPipeType;
  G4double        aper1;
  G4double        aper2;
  G4double        aper3;
  G4double        aper4;
  G4Material*     vacuumMaterial;
  G4double        beamPipeThickness;
  G4Material*     beamPipeMaterial;
};






#endif
