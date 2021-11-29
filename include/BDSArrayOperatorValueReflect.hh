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

/**
 * @brief Reflect field component in individual dimensions.
 * 
 * @author Laurie Nevay
 */

class BDSArrayOperatorValueReflect: public BDSArrayOperatorValue
{
public:
  BDSArrayOperatorValueReflect():
    multiplier{1.0,1.0,1.0,1.0}
  {;}
  explicit BDSArrayOperatorValueReflect(G4bool xyzt[4]):
    BDSArrayOperatorValueReflect(xyzt[0], xyzt[1], xyzt[2], xyzt[3])
  {;}
  BDSArrayOperatorValueReflect(G4bool x,
                               G4bool y,
                               G4bool z,
                               G4bool t):
    BDSArrayOperatorValueReflect()
  {
    multiplier[0] = x ? -1.0 : 1.0;
    multiplier[1] = y ? -1.0 : 1.0;
    multiplier[2] = z ? -1.0 : 1.0;
    multiplier[3] = t ? -1.0 : 1.0;
  }
  virtual ~BDSArrayOperatorValueReflect(){;}
  
  /// Return a name of the operator for feedback to the user in print out.
  virtual G4String Name() const {return name;}
  
  virtual BDSFieldValue Apply(BDSFieldValue v,
                              G4int /*xInd*/,
                              G4int yInd = 0,
                              G4int zInd = 0,
                              G4int tInd = 0) const
  {
    yInd = 2; zInd = 3; tInd = 4;// to retain default values and prevent compiler warnings
    return BDSFieldValue(v.x()*multiplier[0], v.y()*multiplier[1], v.z()*multiplier[2]);
  }
  
private:
  G4String name;
  /// This has to match the type of the field value components (e.g. double or float) to avoid
  /// a possible narrowing conversion.
  FIELDTYPET multiplier[4];
};

#endif
