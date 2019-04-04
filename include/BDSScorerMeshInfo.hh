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

#ifndef BDSSCORERMESHINFO_H
#define BDSSCORERMESHINFO_H

#include "globals.hh"         // geant4 types / globals

class BDSScorerMeshInfo
{
public:
  /// Constructor.
  BDSScorerMeshInfo(const GMAD::ScorerMesh& scorer);

  G4String name;
  G4int    nBinsX;
  G4int    nBinsY;
  G4int    nBinsZ;
  G4double xLow;
  G4double xHigh;
  G4double yLow;
  G4double yHigh;
  G4double zLow;
  G4double zHigh;

  inline G4double ScoringBoxX() const {return 0.5*(xHigh-xLow);}
  inline G4double ScoringBoxY() const {return 0.5*(yHigh-yLow);}
  inline G4double ScoringBoxZ() const {return 0.5*(zHigh-zLow);}

private:
  /// Private default constructor as unused.
  BDSScorerMeshInfo() = delete;
};

#endif
