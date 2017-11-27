/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSBEAMLINESET_H
#define BDSBEAMLINESET_H

class BDSBeamline;

/**
 * @brief Simple struct to return a beamline plus associated beam lines.
 *
 * @author L. Nevay
 */

class BDSBeamlineSet
{
public:
  BDSBeamlineSet(){;}
  ~BDSBeamlineSet(){;}

  /// Destroy objects pointed to by this instance.
  void DeleteContents();

  BDSBeamline* massWorld              = nullptr;
  BDSBeamline* curvilinearWorld       = nullptr;
  BDSBeamline* curvilinearBridgeWorld = nullptr;
  BDSBeamline* endPieces              = nullptr;
};

#endif
