/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSTUBE_H
#define BDSTUBE_H

#include "G4String.hh"
#include "G4TessellatedSolid.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <vector>

/**
 * @brief 
 *
 * @author Laurie Nevay
 */

class BDSTube: public G4TessellatedSolid
{
public:
  BDSTube(const G4String& nameIn,
	  G4double        fullLengthIn,
	  const std::vector<G4TwoVector>& startingPoints);

  BDSTube(const G4String& nameIn,
	  G4double        dZNegative,
	  G4double        dZPositive,
	  const std::vector<G4TwoVector>& startingPoints,
	  const std::vector<G4TwoVector>& finishingPoints);

  virtual ~BDSTube(){;}
};

#endif
