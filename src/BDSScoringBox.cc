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
#include "BDSScoringBox.hh"

#include "G4RotationMatrix.hh"

BDSScoringBox::BDSScoringBox(const G4String& name,
			     const G4Transform3D& placementTransform):
  BDSScoringBox(name, placementTransform.getTranslation(), placementTransform.getRotation())
{;}

BDSScoringBox::BDSScoringBox(const G4String& name,
			     const G4ThreeVector& translation,
			     const G4RotationMatrix& rotation):
  G4ScoringBox(name)
{
  fRotationMatrix = new G4RotationMatrix(rotation);
  fCenterPosition = translation;
}
