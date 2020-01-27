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
#include "G4Types.hh"

#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"

class BDSAcceleratorComponent;

class BDSLinkOpaqueBox: public BDSGeometryComponent
{
public:
  BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
		   G4int indexIn);
  virtual ~BDSLinkOpaqueBox() noexcept = default;

  //! Default constructor
  BDSLinkOpaqueBox() = delete;

  /// Copy constructor
  BDSLinkOpaqueBox(const BDSLinkOpaqueBox &other) = default;

  /// Move constructor
  //BDSLinkOpaqueBox(BDSLinkOpaqueBox &&other) noexcept = default;

  /// Copy assignment operator
  BDSLinkOpaqueBox& operator=(const BDSLinkOpaqueBox &other) = default;

  /// Move assignment operator
  //BDSLinkOpaqueBox& operator=(BDSLinkOpaqueBox &&other) noexcept = default;

private:
  BDSAcceleratorComponent* component;
  G4int index;
  G4ThreeVector toStart;
};

#endif
