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
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <utility>               // for std::pair

BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::instance = nullptr;

BDSBeamPipeFactoryCircular* BDSBeamPipeFactoryCircular::Instance()
{
  if (instance == nullptr)
    {instance = new BDSBeamPipeFactoryCircular();}
  return instance;
}

BDSBeamPipeFactoryCircular::BDSBeamPipeFactoryCircular()
{;}

BDSBeamPipeFactoryCircular::~BDSBeamPipeFactoryCircular()
{
  instance = nullptr;
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipe(G4String    nameIn,
							G4double    lengthIn,
							G4double    aper1In,
							G4double    /*aper2In*/,
							G4double    /*aper3In*/,
							G4double    /*aper4In*/,
							G4Material* vacuumMaterialIn,
							G4double    beamPipeThicknessIn,
							G4Material* beamPipeMaterialIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
   // clean up after last usage
  CleanUp();

  // build the solids
  vacuumSolid   = new G4Tubs(nameIn + "_vacuum_solid",      // name
			     0,                             // inner radius
			     aper1In,                       // outer radius
			     lengthIn*0.5-2*lengthSafety,   // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  beamPipeSolid = new G4Tubs(nameIn + "_pipe_solid",        // name
			     aper1In + lengthSafety,        // inner radius + length safety to avoid overlaps
			     aper1In + beamPipeThicknessIn, // outer radius
			     (lengthIn*0.5)-2*lengthSafety, // half length
			     0,                             // rotation start angle
			     CLHEP::twopi);                 // rotation finish angle
  
  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  containerSolid = new G4Tubs(nameIn + "_container_solid",  // name
			      0,                            // inner radius
			      containerRadius,              // outer radius
			      (lengthIn*0.5)-lengthSafety,  // half length - must fit within magnet / outer geometry
			      0,                            // rotation start angle
			      CLHEP::twopi);                // rotation finish angle
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn,
				 lengthIn, containerRadius);
}

BDSBeamPipe* BDSBeamPipeFactoryCircular::CreateBeamPipe(G4String      nameIn,
							G4double      lengthIn,
							G4ThreeVector inputFaceNormalIn,
							G4ThreeVector outputFaceNormalIn,
							G4double      aper1In,
							G4double      /*aper2In*/,
							G4double      /*aper3In*/,
							G4double      /*aper4In */,
							G4Material*   vacuumMaterialIn,
							G4double      beamPipeThicknessIn,
							G4Material*   beamPipeMaterialIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
   // clean up after last usage
  CleanUp();
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;

  G4double containerRadius = aper1In + beamPipeThicknessIn + lengthSafety;
  
  CreateGeneralAngledSolids(nameIn, lengthIn, aper1In, beamPipeThicknessIn,
			    inputFaceNormal, outputFaceNormal);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn,
				 lengthIn, containerRadius);
}

/// only the solids are unique, once we have those, the logical volumes and placement in the
/// container are the same.  group all this functionality together
BDSBeamPipe* BDSBeamPipeFactoryCircular::CommonFinalConstruction(G4String    nameIn,
								 G4Material* vacuumMaterialIn,
								 G4Material* beamPipeMaterialIn,
								 G4double    lengthIn,
								 G4double    containerRadiusIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  allSolids.push_back(vacuumSolid);
  allSolids.push_back(beamPipeSolid);
  // prepare a longer container subtraction solid
  // doesn't have to be angled as it's only used for transverse subtraction
  containerSubtractionSolid = new G4Tubs(nameIn + "_container_sub_solid",// name
					 0,                              // inner radius
					 containerRadiusIn,              // outer radius
					 lengthIn*4,                     // full length for unambiguous subtraction
					 0,                              // rotation start angle
					 CLHEP::twopi);                  // rotation finish angle

  BDSBeamPipeFactoryBase::CommonConstruction(nameIn, vacuumMaterialIn,
					     beamPipeMaterialIn, lengthIn);

  // record extents
  BDSExtent ext = BDSExtent(containerRadiusIn, containerRadiusIn, lengthIn*0.5);

  return BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(ext, containerRadiusIn);
}

/// the angled ones have degeneracy in the geant4 solids they used so we can avoid code duplication
/// by grouping common construction tasks
void BDSBeamPipeFactoryCircular::CreateGeneralAngledSolids(G4String      nameIn,
							   G4double      lengthIn,
							   G4double      aper1In,
							   G4double      beamPipeThicknessIn,
							   G4ThreeVector inputfaceIn,
							   G4ThreeVector outputfaceIn)
{
  // build the solids
  vacuumSolid   = new G4CutTubs(nameIn + "_vacuum_solid",      // name
				0,                             // inner radius
				aper1In,                       // outer radius
				lengthIn*0.5-2*lengthSafety,   // half length
				0,                             // rotation start angle
				CLHEP::twopi,                  // rotation finish angle
				inputfaceIn,                   // input face normal
				outputfaceIn );                // output face normal
  
  // beampipesolid created as subtraction since direct G4CutTubs creation created scattering in sector bends. not really understood
  
  G4VSolid* inside = new G4CutTubs(nameIn + "_pipe_inner_solid", // name
				   0,                               // inner radius + length safety to avoid overlaps
				   aper1In + lengthSafety,          // outer radius
				   lengthIn,                        // half length
				   0,                               // rotation start angle
				   CLHEP::twopi,                    // rotation finish angle
				   inputfaceIn,                     // input face normal
				   outputfaceIn );
  G4VSolid* outer  = new G4CutTubs(nameIn + "_pipe_outer_solid",  // name
				   0,                               // inner radius + length safety to avoid overlaps
				   aper1In + beamPipeThicknessIn,   // outer radius
				   lengthIn*0.5 - lengthSafety,     // half length
				   0,                               // rotation start angle
				   CLHEP::twopi,                    // rotation finish angle
				   inputfaceIn,                     // input face normal
				   outputfaceIn);
  allSolids.push_back(inside);
  allSolids.push_back(outer);
  
  beamPipeSolid = new G4SubtractionSolid(nameIn + "_pipe_solid",
					 outer,
					 inside);

  containerSolid = new G4CutTubs(nameIn + "_container_solid",  // name
				 0,                            // inner radius
				 aper1In + beamPipeThicknessIn + lengthSafety,  // outer radius
				 lengthIn*0.5-lengthSafety,    // half length
				 0,                            // rotation start angle
				 CLHEP::twopi,                 // rotation finish angle
				 inputfaceIn,                  // input face normal
				 outputfaceIn);                // rotation finish angle
}
