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
#ifndef BDSCOLLIMATORRECTANGULAR_H
#define BDSCOLLIMATORRECTANGULAR_H

#include "BDSCollimatorBase.hh"

/**
 * @brief A class for a rectangular collimator.
 */

class BDSCollimatorRectangular: public BDSCollimatorBase
{
public:
  BDSCollimatorRectangular(G4String name,
			   G4double length,
			   G4double horizontalWidth,
			   G4double xApertureIn        = 0,
			   G4double yApertureIn        = 0,
			   G4double xOutApertureIn     = 0,
			   G4double yOutApertureIn     = 0,
			   G4String collimatorMaterial = "copper",
			   G4String vacuumMaterial     = "vacuum",
			   G4String colour             = "collimator");
  virtual ~BDSCollimatorRectangular(){;};

  virtual void BuildInnerCollimator();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorRectangular();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorRectangular& operator=(const BDSCollimatorRectangular&) = delete;
  BDSCollimatorRectangular(BDSCollimatorRectangular&) = delete;
  ///@}
};

#endif
