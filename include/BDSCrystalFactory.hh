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
#ifndef BDSCRYSTALFACTORY_H
#define BDSCRYSTALFACTORY_H

#include "BDSCrystal.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4RotationMatrix.hh"

class BDSCrystalInfo;

class G4LogicalVolume;
class G4Material;
class G4PVPlacement;
class G4UserLimits;
class G4VSolid;

/**
 * @brief Abstract base class for crystal factory classes.
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of crystal geometry required.
 * 
 * @author Laurie Nevay
 */

class BDSCrystalFactory
{
public:
  BDSCrystalFactory();
  virtual ~BDSCrystalFactory(){;}

  /// Main interface to create a crystal.
  BDSCrystal* CreateCrystal(const G4String&       nameIn,
			    const BDSCrystalInfo* recipe);

private:
  /// Initialiser and can be used to reset factory pointers after use
  void CleanUp();
  
  /// Common construction tasks.
  void CommonConstruction(G4String    nameIn,
			  G4Material* crystalMaterialIn,
			  G4double    length);

  /// build beampipe and register logical volumes
  BDSCrystal* BuildCrystalObject(const BDSExtent& extent);
  
  void SetVisAttributes();                    ///< Set visual attributes.  
  void SetUserLimits(const G4double& length); ///< Set user limits.

  /// Create box geometry for a crystal.
  BDSCrystal* CreateCrystalBox(const G4String&       nameIn,
			       const BDSCrystalInfo* recipe);
  
  const G4double   maxStepFactor;      ///< Fraction of length for maximum step in user limits.
  const G4double   nSegmentsPerCircle; ///< For visualisation improvement.
  
  G4VSolid*        crystalSolid;
  G4LogicalVolume* crystalLV;
  G4ThreeVector    placementOffset;

  /// @{ For non standard parts for easy registration - ie not the specific ones above.
  std::vector<G4LogicalVolume*>   allLogicalVolumes;
  std::vector<G4VPhysicalVolume*> allPhysicalVolumes;
  std::vector<G4RotationMatrix*>  allRotationMatrices;
  std::vector<G4VSolid*>          allSolids;
  std::vector<G4UserLimits*>      allUserLimits;
  std::vector<G4VisAttributes*>   allVisAttributes;
  /// @}
};

#endif
