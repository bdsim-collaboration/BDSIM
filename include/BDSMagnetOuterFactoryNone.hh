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
#ifndef BDSMAGNETOUTERFACTORYNONE_H
#define BDSMAGNETOUTERFACTORYNONE_H

#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryBase.hh"

#include "globals.hh"           // geant4 globals / types

class BDSBeamPipe;
class G4Material;

/**
 * @brief Factory that produces null outer geometry.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryNone: public BDSMagnetOuterFactoryBase
{
public:
  static BDSMagnetOuterFactoryNone* Instance(); /// singleton pattern
  
  virtual ~BDSMagnetOuterFactoryNone();
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String     name,                  // name
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     horizontalWidth,       // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4double     angleIn,               // input face angle w.r.t. chord
					   G4double     angleOut,              // output face angle w.r.t. chord
					   G4bool       yokeOnLeft,            // build magnet yoke on left of bend
					   G4bool       hStyle,                 // H style magnet (c shaped if not)
					   G4Material*  outerMaterial = nullptr,// material for outer volume
					   G4bool       buildEndPiece = false,
					   G4double     vhRatio       = 1.0,    // ratio of vertical to horizontal proportions
					   G4double     coilWidthFraction  = 0.65,
					   G4double     coilHeightFraction = 0.8
					   );

  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String     name,                  // name
						G4double     length,                // length [mm]
						BDSBeamPipe* beamPipe,              // beampipe
						G4double     horizontalWidth,       // full width
						G4double     containerLength,       // full length to make AccComp container
						G4double     angleIn,               // input face angle w.r.t. chord
						G4double     angleOut,              // output face angle w.r.t. chord
						G4bool       yokeOnLeft,            // build magnet yoke on left of bend
						G4bool       hStyle,                 // H style magnet (c shaped if not)
						G4Material*  outerMaterial = nullptr,// material for outer volume
						G4bool       buildEndPiece = false,
						G4double     vhRatio       = 1.0,    // ratio of vertical to horizontal proportions
						G4double     coilWidthFraction  = 0.65,
						G4double     coilHeightFraction = 0.8
						);
  
  /// quadrupole outer volume
  virtual BDSMagnetOuter* CreateQuadrupole(G4String     name,                  // name
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     horizontalWidth,       // full width
					   G4double     containerLength,       // full length to make AccComp container
					   G4Material*  outerMaterial = nullptr,// material for outer volume
					   G4bool       buildEndPiece = false
					   );
  

  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     horizontalWidth,       // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr,// material for outer volume
					  G4bool       buildEndPiece = false
					  );

  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     horizontalWidth,       // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false
					 );
  
  /// decapole outer volume
  virtual BDSMagnetOuter* CreateDecapole(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     horizontalWidth,       // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false
					 );
  
  /// solenoid  outer volume
  virtual BDSMagnetOuter* CreateSolenoid(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     horizontalWidth,       // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false
					 );
  
  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSMagnetOuter* CreateMultipole(G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     horizontalWidth,       // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr,// material for outer volume
					  G4bool       buildEndPiece = false
					  );
  
  /// RF cavity outer volume
  virtual BDSMagnetOuter* CreateRfCavity(G4String     name,                  // name
					 G4double     length,                // length [mm]
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     horizontalWidth,       // full width
					 G4double     containerLength,       // full length to make AccComp container
					 G4Material*  outerMaterial = nullptr,// material for outer volume
					 G4bool       buildEndPiece = false
					 );

  /// muon spoiler outer volume
  virtual BDSMagnetOuter* CreateMuSpoiler(G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     horizontalWidth,       // full width
					  G4double     containerLength,       // full length to make AccComp container
					  G4Material*  outerMaterial = nullptr,// material for outer volume
					  G4bool       buildEndPiece = false
					  );
  
  /// horizontal and vertical kicker outer volume
  virtual BDSMagnetOuter* CreateKicker(G4String     name,                  // name
				       G4double     length,                // length [mm]
				       BDSBeamPipe* beamPipe,              // beampipe
				       G4double     horizontalWidth,       // full width
				       G4double     containerLength,       // full length to make AccComp container
				       G4bool       yokeOnLeft,            // where c-shaped yoke is
				       G4bool       vertical = true,       // is it a vertical kicker?
				       G4Material*  outerMaterial = nullptr,// material for outer volume
				       G4bool       buildEndPiece = false,
				       G4bool       hStyle             = false,
				       G4double     vhRatio            = 1.0,
				       G4double     coilWidthFraction  = 0.65,
				       G4double     coilHeightFraction = 0.8
				       );
  
private:
  BDSMagnetOuterFactoryNone(); //private constructor as singleton
  static BDSMagnetOuterFactoryNone* _instance;
};

#endif
