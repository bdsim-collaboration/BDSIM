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
#ifndef BDSPCLTUBE_H
#define BDSPCLTUBE_H

class G4VSolid;

/** 
 * @brief PostCollisionLine Tube
 * 
 * @author Lawrence Deacon <l.deacon@cern.ch>>
 */

class BDSPCLTube
{
public:
  BDSPCLTube(G4double aperX, G4double aperYUp, G4double aperYDown, G4double aperDy, G4double thickness, G4double length, G4String name);
  ~BDSPCLTube();
  G4VSolid* GetSolid();
  void Build();
  void BuildHollow();
private:
  G4double itsSizeX;
  G4double itsSizeYUp;
  G4double itsSizeYDown;
  G4double itsSizeDy;
  G4double itsThickness;
  G4double itsLength;
  G4String itsName;

  G4VSolid* upper_solid;
  G4VSolid* middle_solid;
  G4VSolid* lower_solid;
  G4VSolid* temp_solid;
  G4VSolid* solid;


};

#endif
