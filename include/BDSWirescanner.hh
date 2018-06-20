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
#ifndef BDSWIRESCANNER_H
#define BDSWIRESCANNER_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

/**
 * @brief Degrader based on wedge design used in the PSI medical accelerator.
 * 
 * @author Will Shields
 */

class BDSWirescanner: public BDSAcceleratorComponent
{
public:
    BDSWirescanner(G4String name,
	      G4double   length,
	      G4double   outerDiameter,
          G4double   wireDiameter,
          G4double   wireLength,
          G4double   wirescannerOffset,
          G4double   wirescannerRotx,
          G4double   wirescannerRoty,
          BDSBeamPipeInfo*   bdsBeamPipeInfo);
  virtual ~BDSWirescanner();
  
protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();
  
  G4double outerDiameter;
  G4double wireDiameter;
  G4double wireLength;
  G4double wirescannerOffset;
  G4double wirescannerRotx;
  G4double wirescannerRoty;
private:
  /// Private default constructor to force the use of the supplied one.
  BDSWirescanner() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSWirescanner& operator=(const BDSWirescanner&) = delete;
    BDSWirescanner(BDSWirescanner&) = delete;
  ///@}
};

#endif
