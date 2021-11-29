/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#ifndef BDSARRAYOPERATORCOORDREFLECT_H
#define BDSARRAYOPERATORCOORDREFLECT_H
#include "BDSArrayOperatorIndex.hh"

#include "G4Types.hh"

#include <cmath>

/**
 * @brief 1D array for completeness in array system.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorIndexReflect: public BDSArrayOperatorIndex
{
public:
  BDSArrayOperatorIndexReflect():
    xyzt{false, false, false, false}
  {;}
  explicit BDSArrayOperatorIndexReflect(G4bool xyzt[4]):
    BDSArrayOperatorIndexReflect(xyzt[0], xyzt[1], xyzt[2], xyzt[3])
  {;}
  BDSArrayOperatorIndexReflect(G4bool x,
                               G4bool y,
                               G4bool z,
                               G4bool t):
    xyzt{x,y,z,t}
  {;}
  virtual ~BDSArrayOperatorIndexReflect(){;}
  
  virtual G4String Name() const {return "Reflect";}
  
  virtual void Apply(G4int& x,
                     G4int& y,
                     G4int& z,
                     G4int& t) const
  {
    x = xyzt[0] ? std::abs(x) : x;
    y = xyzt[1] ? std::abs(y) : y;
    z = xyzt[2] ? std::abs(z) : z;
    t = xyzt[3] ? std::abs(t) : t;
  }
  
  virtual void ApplyX(G4int& x) const {x = xyzt[0] ? std::abs(x) : x;}
  virtual void ApplyY(G4int& y) const {y = xyzt[1] ? std::abs(y) : y;}
  virtual void ApplyZ(G4int& z) const {z = xyzt[2] ? std::abs(z) : z;}
  virtual void ApplyT(G4int& t) const {t = xyzt[3] ? std::abs(t) : t;}
  
  virtual void TransformLimits(G4double& xMin, G4double& xMax,
                               G4double& yMin, G4double& yMax,
                               G4double& zMin, G4double& zMax,
                               G4double& tMin, G4double& tMax) const
  {
    xMin = -std::abs(xMax);
    yMin = -std::abs(yMax);
    zMin = -std::abs(zMax);
    tMin = -std::abs(tMax);
  }
  
private:
  G4bool xyzt[4];
};

#endif
