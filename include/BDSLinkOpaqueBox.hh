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
#ifndef LINKOPAQUEBOX_H
#define LINKOPAQUEBOX_H

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include "BDSGeometryComponent.hh"

class BDSAcceleratorComponent;

class BDSLinkOpaqueBox: public BDSGeometryComponent
{
public:
  BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
		   G4double tiltIn,
		   G4int indexIn);
  virtual ~BDSLinkOpaqueBox(){;}

  /// Default constructor
  BDSLinkOpaqueBox() = delete;

  /// Copy constructor
  BDSLinkOpaqueBox(const BDSLinkOpaqueBox &other) = default;
  /// Copy assignment operator
  BDSLinkOpaqueBox& operator=(const BDSLinkOpaqueBox &other) = default;

  inline G4int Index() const {return index;}
  inline const G4ThreeVector& OffsetToStart()    const {return offsetToStart;}
  inline const G4Transform3D& TransformToStart() const {return transformToStart;}

  void PlaceOutputSampler(G4int ID){;}

private:
  BDSAcceleratorComponent* component;
  G4int index;
  G4ThreeVector offsetToStart;
  G4Transform3D transformToStart;
};

#endif
