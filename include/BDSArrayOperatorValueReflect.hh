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
#ifndef BDSARRAYOPERATORVALUEREFLECT_H
#define BDSARRAYOPERATORVALUEREFLECT_H
#include "BDSArrayOperatorValue.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <string>

/**
 * @brief Reflect field component in individual dimensions.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueReflect: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueReflect():
    BDSArrayOperatorValue("Reflect(None)"),
    xyzt{false, false, false}
  {;}
  explicit BDSArrayOperatorValueReflect(G4bool xyzt[4]):
    BDSArrayOperatorValueReflect(xyzt[0], xyzt[1], xyzt[2], xyzt[3])
  {;}
  BDSArrayOperatorValueReflect(G4bool x,
                               G4bool y,
                               G4bool z,
                               G4bool /*t*/):
    BDSArrayOperatorValueReflect()
  {
    xyzt[0] = x;
    xyzt[1] = y;
    xyzt[2] = z;
    
    G4String newName = "Reflect(";
    for (const auto& v : xyzt)
      {newName += std::to_string(v);}
    newName += ")";
    name = newName;
  }
  virtual ~BDSArrayOperatorValueReflect(){;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int xInd,
                              G4int yInd = 0,
                              G4int zInd = 0,
                              G4int tInd = 0) const
  {
    G4double xM = (xInd < 0) && xyzt[0] ? -1.0 : 1.0;
    G4double yM = (yInd < 0) && xyzt[1] ? -1.0 : 1.0;
    G4double zM = (zInd < 0) && xyzt[2] ? -1.0 : 1.0;
    tInd = 4;// to retain default values and prevent compiler warnings
    return BDSFieldValue(v.x()*xM, v.y()*yM, v.z()*zM);
  }
  
private:
  G4bool xyzt[3];
};

#endif
