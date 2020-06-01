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
#include "BDSExtent.hh"
#include "BDSFieldMagInterpolated.hh"
#include "BDSFieldMagInterpolated2Layer.hh"

#include "G4ThreeVector.hh"

BDSFieldMagInterpolated2Layer::BDSFieldMagInterpolated2Layer(BDSFieldMagInterpolated* mainFieldIn,
							     BDSFieldMagInterpolated* subFieldIn):
  mainField(mainFieldIn),
  subField(subFieldIn),
  subFieldExtent(subFieldIn->Extent())
{;}

BDSFieldMagInterpolated2Layer::~BDSFieldMagInterpolated2Layer()
{
  delete mainField;
  delete subField;
}

G4ThreeVector BDSFieldMagInterpolated2Layer::GetField(const G4ThreeVector& position,
						      const G4double       t) const
{
  if (subFieldExtent.Encompasses(position))
    {return subField->GetField(position, t);}
  else
    {return mainField->GetField(position, t);}
}
