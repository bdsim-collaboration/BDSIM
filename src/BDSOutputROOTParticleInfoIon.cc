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

#include "BDSOutputROOTParticleInfoIon.hh"

#include <string>

ClassImp(BDSOutputROOTParticleInfoIon)

BDSOutputROOTParticleInfoIon::BDSOutputROOTParticleInfoIon():
  a(1),
  z(1)
{;}

BDSOutputROOTParticleInfoIon::BDSOutputROOTParticleInfoIon(const std::string& nameIn,
							   const int&         chargeIn,
							   const double&      massIn,
							   const int&         aIn,
							   const int&         zIn):
  BDSOutputROOTParticleInfo(nameIn, chargeIn, massIn),
  a(aIn),
  z(zIn)
{;}
