/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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
#ifndef BDSSCORERFACTORY_H
#define BDSSCORERFACTORY_H

#include "BDSScorerType.hh"

#include "globals.hh"
#include "G4String.hh"

class BDSHistBinMapper3D;
class BDSScorer;
class BDSScorerInfo;
class G4VPrimitiveScorer;

class G4Material;

/**
 * @brief Create primitive scorers on demand.
 *
 * @author Robin Tesse
 */

class BDSScorerFactory
{
public:
  BDSScorerFactory();
  ~BDSScorerFactory(){;}
  
  /// Main function to create a scorer.
  G4VPrimitiveScorer* CreateScorer(const BDSScorerInfo*      info,
				   const BDSHistBinMapper3D* mapper,
				   G4double*                 unit = nullptr);
  
private:  
  ///@{ Unused default constructors
  BDSScorerFactory(const BDSScorerFactory&) = delete;
  BDSScorerFactory& operator=(const BDSScorerFactory&) = delete;
  ///@}

  /// Construct the primitive scorer required.
  G4VPrimitiveScorer* GetAppropriateScorer(G4String                  name,
					   const BDSScorerType       scorerType,
					   G4String                  filename,
					   const BDSHistBinMapper3D* mapper,
					   G4double*                 unit = nullptr);
};

#endif
