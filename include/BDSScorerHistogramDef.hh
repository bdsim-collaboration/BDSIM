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

#ifndef BDSSCORERHISTOGRAMDEF_H
#define BDSSCORERHISTOGRAMDEF_H

#include "BDSHistBinMapper3D.hh"
#include "BDSScorerMeshInfo.hh"

#include "globals.hh"         // geant4 types / globals

class BDSScorerHistogramDef: public BDSScorerMeshInfo
{
public:
  BDSScorerHistogramDef(const BDSScorerMeshInfo&  meshInfo,
			const G4String&           uniqueNameIn,
			const BDSHistBinMapper3D& coordinateMapperIn);


  virtual ~BDSScorerHistogramDef(){;}

  G4String uniqueName; ///< Unique name of mesh/scorer -> slash required by Geant4.
  G4String outputName; ///< Copy of unique name that's safe for output.
  BDSHistBinMapper3D coordinateMapper; ///< Coordinate mapper - this class owns it.

private:
  BDSScorerHistogramDef() = delete;
};

#endif
