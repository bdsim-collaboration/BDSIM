/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSMagnetOuterFactoryPolesFacetCrop.hh"


BDSMagnetOuterFactoryPolesFacetCrop* BDSMagnetOuterFactoryPolesFacetCrop::_instance = nullptr;

BDSMagnetOuterFactoryPolesFacetCrop* BDSMagnetOuterFactoryPolesFacetCrop::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryPolesFacetCrop();}
  return _instance;
}

BDSMagnetOuterFactoryPolesFacetCrop::BDSMagnetOuterFactoryPolesFacetCrop():
  BDSMagnetOuterFactoryPolesFacetCommon(2)
{;}

BDSMagnetOuterFactoryPolesFacetCrop::~BDSMagnetOuterFactoryPolesFacetCrop()
{
  _instance = nullptr;
}

void BDSMagnetOuterFactoryPolesFacetCrop::CalculateStartAngles()
{
  polyStartAngle = segmentAngle*0.25;
  poleIntersectionStartAngle = -segmentAngle*0.25 - CLHEP::halfpi;
}
