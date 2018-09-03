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
#ifndef BDSBEAMPIPEFACTORYELLIPTICAL_H
#define BDSBEAMPIPEFACTORYELLIPTICAL_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for CLIC post collision line beam pipes.
 * 
 * Singleton pattern.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryClicPCL: public BDSBeamPipeFactoryBase
{
public:
  static BDSBeamPipeFactoryClicPCL* Instance(); /// Singleton accessor.
  
  virtual ~BDSBeamPipeFactoryClicPCL();

  virtual BDSBeamPipe* CreateBeamPipe(G4String    nameIn,
				      G4double    lengthIn,
				      G4double    aper1               = 0,
				      G4double    aper2               = 0,
				      G4double    aper3               = 0,
				      G4double    aper4               = 0,
				      G4Material* vacuumMaterialIn    = nullptr,
				      G4double    beamPipeThicknessIn = 0,
				      G4Material* beamPipeMaterialIn  = nullptr);
  
  virtual BDSBeamPipe* CreateBeamPipe(G4String      nameIn,
				      G4double      lengthIn,
				      G4ThreeVector inputFaceNormalIn,
				      G4ThreeVector outputFaceNormalIn,
				      G4double      aper1               = 0,
				      G4double      aper2               = 0,
				      G4double      aper3               = 0,
				      G4double      aper4               = 0,
				      G4Material*   vacuumMaterialIn    = nullptr,
				      G4double      beamPipeThicknessIn = 0,
				      G4Material*   beamPipeMaterialIn  = nullptr);

private:
  BDSBeamPipeFactoryClicPCL(); ///< Private default constructor - singelton pattern.
  static BDSBeamPipeFactoryClicPCL* instance; ///< Singleton instance.

  //abstract common build features to one function
  //use member variables unique to this factory to pass them around

  /// only the solids are unique, once we have those, the logical volumes and placement in the
  /// container are the same.  group all this functionality together
  BDSBeamPipe* CommonFinalConstruction(G4String    nameIn,
				       G4Material* vacuumMaterialIn,
				       G4Material* beamPipeMaterialIn,
				       G4double    lengthIn,
				       G4double    aper1In,
				       G4double    aper2In,
				       G4double    beamPipeThicknessIn);

  /// The angled ones have degeneracy in the geant4 solids they used so we can
  /// avoid code duplication by grouping common construction tasks.
  void CreateGeneralAngledSolids(G4String      nameIn,
				 G4double      lengthIn,
				 G4double      aper1In,
				 G4double      aper2In,
				 G4double      beamPipeThicknessIn,
				 G4ThreeVector inputfaceIn,
				 G4ThreeVector outputfaceIn);
};
  
#endif
