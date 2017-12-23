/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSMAGNETOUTERFACTORYBASE_H
#define BDSMAGNETOUTERFACTORYBASE_H

#include "BDSExtent.hh"
#include "BDSMagnetOuter.hh"

#include "globals.hh"         // geant4 globals / types

#include <vector>

class BDSBeamPipe;
class BDSGeometryComponent;

class G4Colour;
class G4Material;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief Abstract base class for magnet outer volume factories.
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of outer logical volume required. 
 * 
 * Uses Geant4 default units by default, ie mm, rad (not metres)
 *
 * Strictly the naming should contain 'Outer' at the end to distinguish
 * that the function doesn't create the whole magnet, however this is
 * implied by the factory name and omitting it will reduce the name length.
 *
 * Each factory should implement a method for every type of magnet - ie for
 * each class that inherits BDSMagnet.
 * 
 * NOTE each derived class is expected to be a singleton but this can't be specified here
 * as it'd refer to the abstract base class - must be implemented in each derived class. 
 * 'In' in argument variables is used to distinguish between that and possible
 * internal member variables with the same name - avoiding 'itsVariable'.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryBase
{
public:
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String     name,                   // name
					   G4double     length,                 // full length [mm]
					   BDSBeamPipe* beamPipe,               // beampipe
					   G4double     outerDiameter,          // full width
					   G4double     containerLength,        // full length to make AccComp container
					   G4double     angleIn,                // input face angle w.r.t. chord
					   G4double     angleOut,               // output face angle w.r.t. chord
					   G4bool       yokeOnLeft,             // build magnet yoke on left of bend
					   G4bool       hStyle,                 // H style magnet (c shaped if not)
					   G4Material*  outerMaterial = nullptr,// material for outer volume
					   G4bool       buildEndPiece = false,  // build and end piece
					   G4double     vhRatio       = 1.0,    // ratio of vertical to horizontal proportions
					   G4double     coilWidthFraction  = 0.65,
					   G4double     coilHeightFraction = 0.8
					   ) = 0;
  
  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String     name,              // name
						G4double     length,            // full length [mm]
						BDSBeamPipe* beamPipe,          // beampipe
						G4double     outerDiameter,     // full width
						G4double     containerLength,   // full length to make AccComp container
						G4double     angleIn,           // input face angle w.r.t. chord
						G4double     angleOut,          // output face angle w.r.t. chord
						G4bool       yokeOnLeft,        // build magnet yoke on left of bend
						G4bool       hStyle,                 // H style magnet (c shaped if not)
						G4Material*  outerMaterial = nullptr,// material for outer volume
						G4bool       buildEndPiece = false,  // build and end piece
						G4double     vhRatio       = 1.0,    // ratio of vertical to horizontal proportions
						G4double     coilWidthFraction  = 0.65,
						G4double     coilHeightFraction = 0.8
						) = 0;
  
  
  /// quadrupole outer volume
  virtual BDSMagnetOuter* CreateQuadrupole(G4String     name,                   // name
					   G4double     length,                 // length [mm]
					   BDSBeamPipe* beamPipe,               // beampipe
					   G4double     outerDiameter,          // full width
					   G4double     containerLength,        // full length to make AccComp container
					   G4Material*  outerMaterial = nullptr,// material for outer volume
					   G4bool       buildEndPiece = false   // build and end piece
					   ) = 0;
  

  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(G4String     name,                 // name
					  G4double     length,               // length [mm]
					  BDSBeamPipe* beamPipe,             // beampipe
					  G4double     outerDiameter,        // full width
					  G4double     containerLength,      // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr,// material for outer volume
					  G4bool       buildEndPiece = false   // build and end piece
					  ) = 0;
  
  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false   // build and end piece
					 ) = 0;

  /// decapole outer volume
  virtual BDSMagnetOuter* CreateDecapole(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false   // build and end piece
					 ) = 0;
  
  /// solenoid  outer volume
  virtual BDSMagnetOuter* CreateSolenoid(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false   // build and end piece
					 ) = 0;
  
  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSMagnetOuter* CreateMultipole(G4String     name,                 // name
					  G4double     length,               // length [mm]
					  BDSBeamPipe* beamPipe,             // beampipe
					  G4double     outerDiameter,        // full width
					  G4double     containerLength,      // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr,// material for outer volume
					  G4bool       buildEndPiece = false   // build and end piece
					  ) = 0;
  
  /// RF cavity outer volume
  virtual BDSMagnetOuter* CreateRfCavity(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     outerDiameter,         // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false   // build and end piece
					 ) = 0;

  /// muon spoiler outer volume
  virtual BDSMagnetOuter* CreateMuSpoiler(G4String     name,                 // name
					  G4double     length,               // length [mm]
					  BDSBeamPipe* beamPipe,             // beampipe
					  G4double     outerDiameter,        // full width
					  G4double     containerLength,      // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr,// material for outer volume
					  G4bool       buildEndPiece = false   // build and end piece
					  ) = 0;
  
  /// horizontal and vertical kicker outer volume
  virtual BDSMagnetOuter* CreateKicker(G4String     name,                  // name
				       G4double     length,                // length [mm]
				       BDSBeamPipe* beamPipe,              // beampipe
				       G4double     outerDiameter,         // full width
				       G4double     containerLength,       // full length to make AccComp container
				       G4bool       vertical = true,       // is it a vertical kicker?
				       G4Material*  outerMaterial = nullptr,// material for outer volume
				       G4bool       buildEndPiece = false,  // build and end piece
				       G4bool       hStyle             = false,
				       G4double     vhRatio            = 1.0,
				       G4double     coilWidthFraction  = 0.65,
				       G4double     coilHeightFraction = 0.8
				       ) = 0;

  /// Empty containers for next use - factories are never deleted so can't rely on scope
  virtual void CleanUp();
  /// Virtual base destructor
  virtual ~BDSMagnetOuterFactoryBase() {}

protected:
  BDSMagnetOuterFactoryBase();

  /// Create logical volumes for yoke, container and magnet container - derived classes can override to
  /// extend as they need.
  virtual void CreateLogicalVolumes(G4String    name,
				    G4Colour*   colour,
				    G4Material* outerMaterial);

  /// Utility function to make cylindrical magnetContainerSolid in the case
  /// of one with angled faces
  void BuildMagnetContainerSolidAngled(G4String      name,
				       G4double      magnetContainerLength,
				       G4double      magnetContainerRadius);

  /// Utility function to make cylindrical magnetContainerSolid in the case
  /// of flat faces
  void BuildMagnetContainerSolidStraight(G4String name,
					 G4double magnetContainerLength,
					 G4double magnetContainerRadius);

  void CreateMagnetContainerComponent();

  /// Copy face normals from members to an instance of outer.
  void SetFaceNormals(BDSMagnetOuter* outer);
  
  // geometric pointers that will be used to pass around components
  // within the factory (as different parts factorised so they can
  // be overridden by the derived classes.
  G4double           lengthSafety;
  G4bool             checkOverlaps; ///< to avoid using globalconstants a lot
  G4bool             visDebug;

  /// For visualisation improvement. Also used for coil end piece geometry
  G4double           nSegmentsPerCircle;
  G4double           maxStepFactor;      ///< for user limits
  G4VSolid*          poleSolid; ///< solid for an individual pole that will be placed multiple times
  G4VSolid*          yokeSolid; ///< solid for outer part that connects all poles
  G4VSolid*          containerSolid;
  G4VSolid*          magnetContainerSolid;
  G4LogicalVolume*   poleLV;
  G4LogicalVolume*   yokeLV;
  G4LogicalVolume*   containerLV;
  G4LogicalVolume*   magnetContainerLV;
  G4VPhysicalVolume* yokePV;
  G4VisAttributes*   outerVisAttributes;

  std::vector<G4LogicalVolume*>   allLogicalVolumes;
  std::vector<G4VPhysicalVolume*> allPhysicalVolumes;
  std::vector<G4RotationMatrix*>  allRotationMatrices;
  std::vector<G4VSolid*>          allSolids;
  std::vector<G4VisAttributes*>   allVisAttributes;

  BDSExtent                       magContExtent;
  BDSGeometryComponent*           magnetContainer;

  G4ThreeVector inputFaceNormal;
  G4ThreeVector outputFaceNormal;

  /// A larger length safety that can be used where tracking accuracy isn't required
  /// or more tolerant geometry is required (1um).
  static G4double const lengthSafetyLarge;
};

#endif
