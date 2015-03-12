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

struct BDSBeamPipeInfo {
  /// default constructor
  BDSBeamPipeInfo();

  /// extra constructor to assign all members at once
  BDSBeamPipeInfo(BDSBeamPipeType beamPipeTypeIn,
		  G4double        aper1In,
		  G4double        aper2In,
		  G4double        aper3In,
		  G4double        aper4In,
		  G4Material*     vacuumMaterialIn,
		  G4double        beamPipeThicknessIn,
		  G4Material*     beamPipeMaterialIn);
		  
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
