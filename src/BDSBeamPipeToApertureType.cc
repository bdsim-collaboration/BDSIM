/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSApertureType.hh"
#include "BDSBeamPipeToApertureType.hh"
#include "BDSBeamPipeType.hh"

#include <map>

static const std::map<BDSBeamPipeType, BDSApertureType> beamPipeToApertureTypes =
  {
    {BDSBeamPipeType::circular, BDSApertureType::circle},
    {BDSBeamPipeType::circularvacuum, BDSApertureType::circle},
    {BDSBeamPipeType::rectangular, BDSApertureType::rectangle},
    {BDSBeamPipeType::elliptical, BDSApertureType::ellipse},
    {BDSBeamPipeType::lhc, BDSApertureType::rectcircle},
    {BDSBeamPipeType::lhcdetailed, BDSApertureType::rectcircle},
    {BDSBeamPipeType::racetrack, BDSApertureType::racetrack},
    {BDSBeamPipeType::octagonal, BDSApertureType::octagon},
    {BDSBeamPipeType::clicpcl, BDSApertureType::clicpcl}
  };

BDSApertureType BDS::ApertureTypeFromBeamPipeType(BDSBeamPipeType bpt)
{
  return beamPipeToApertureTypes.at(bpt);
}
