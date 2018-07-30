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
#ifndef BDSDEGRADER_H
#define BDSDEGRADER_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

/**
 * @brief Degrader based on wedge design used in the PSI medical accelerator.
 * 
 * @author Will Shields
 */

class BDSDegrader: public BDSAcceleratorComponent
{
public:
  BDSDegrader(G4String nameIn, 
	      G4double lengthIn,
	      G4double horizontalWidthIn,
	      G4int    numberWedgesIn,
	      G4double wedgeLengthIn,
	      G4double degraderHeightIn,
	      G4double degraderOffsetIn,
	      G4String degraderMaterialIn = "carbon");
  virtual ~BDSDegrader();
  
protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();
  
  G4double horizontalWidth;
  G4int    numberWedges;
  G4double wedgeLength;
  G4double degraderHeight;
  G4double degraderOffset;
  G4String degraderMaterial; 
  
  bool isOdd(G4int integer)
  {
    if (integer % 2 != 0)
      return true;
    else
      return false;
  }
  
  bool isEven(G4int integer)
  {
    if (integer % 2 == 0)
      return true;
    else
      return false;
  }
  
private:
  /// Private default constructor to force the use of the supplied one.
  BDSDegrader() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSDegrader& operator=(const BDSDegrader&) = delete;
  BDSDegrader(BDSDegrader&) = delete;
  ///@}
};

#endif
