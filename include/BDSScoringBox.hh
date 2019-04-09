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
#ifndef BDSSCORINGBOX_H
#define BDSSCORINGBOX_H

#include "G4ScoringBox.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSScorerMeshInfo;

/**
 * @brief Wrapper for G4ScoringBox to allow full access to placement.
 * 
 * The G4ScoringBox class provides an incomplete interface to the
 * underlying translation 3 vector and rotation matrix. Here, we
 * wrap the class to assign these protected members directly.
 * 
 * @author Laurie Nevay
 */

class BDSScoringBox: public G4ScoringBox
{
public:
  /// Construct with size and divisions from recipe class. Retain full control
  /// over name of mesh.
  BDSScoringBox(const G4String&          name,
		const BDSScorerMeshInfo& recipe,
		const G4Transform3D&     placementTransform);

  /// Construct without size and divisions. Presumed these will be set later
  /// after construction with methods.
  BDSScoringBox(const G4String&      name,
		const G4Transform3D& placementTransform);

  /// Separate constructor with objects that are assigned directly to
  /// protected base class members.
  BDSScoringBox(const G4String&         name,
		const G4ThreeVector&    translation,
		const G4RotationMatrix& rotation);

  virtual ~BDSScoringBox(){;}
};

#endif
