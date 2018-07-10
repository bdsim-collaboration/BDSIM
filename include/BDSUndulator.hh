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
#ifndef BDSUNDULATOR_H
#define BDSUNDULATOR_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

/**
 * @brief Undulator magnet
 * 
 * @author Will Shields
 */

class BDSUndulator: public BDSAcceleratorComponent
{
public:
    BDSUndulator(G4String name,
	      G4double   length,
	      G4double   outerDiameter,
	      G4int      numberWedges,
	      G4double   wedgeLength,
	      G4double   degraderHeight,
	      G4double   degraderOffset,
	      G4String   degraderMaterial     = "carbon");
  virtual ~BDSUndulator();
  
protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();
  
  G4double outerDiameter;
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
  BDSUndulator& operator=(const BDSUndulator&) = delete;
  BDSUndulator(BDSUndulator&) = delete;
  ///@}
};

#endif
