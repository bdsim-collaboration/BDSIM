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
#ifndef BDSCOLLIMATORCRYSTAL_H
#define BDSCOLLIMATORCRYSTAL_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSCrystalPosition.hh"

class BDSBeamPipeInfo;
class BDSCrystal;
class BDSCrystalInfo;

/**
 * @brief A piece of vacuum beam pipe with a crystal for channelling.
 * 
 * @author Laurie Nevay
 */

class BDSCollimatorCrystal: public BDSAcceleratorComponent
{
public:
  BDSCollimatorCrystal(G4String           name,
		       G4double           length,
		       BDSBeamPipeInfo*   beamPipeInfoIn,
		       BDSCrystalPosition crystalPositionIn,
		       BDSCrystalInfo*    crystalInfoLeftIn,
		       BDSCrystalInfo*    crystalInfoRightIn);
  virtual ~BDSCollimatorCrystal();

  /// Override base class version and return crystal material.
  virtual G4String Material() const;

protected:
  /// Construct geometry.
  virtual void Build();

private:
  /// No default constructor.
  BDSCollimatorCrystal() = delete;
  
  /// Void function to fulfill BDSAcceleratorComponent requirements.
  virtual void BuildContainerLogicalVolume(){;}

  BDSCrystalPosition crystalPosition;   ///< Enum for left, right or both crystals.
  BDSCrystalInfo*    crystalInfoLeft;   ///< Model associated with left crystal.
  BDSCrystalInfo*    crystalInfoRight;  ///< Model associated with right crystal.
  BDSCrystal* crystalLeft;
  BDSCrystal* crystalRight;
};

#endif
