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
#include "BDSOutputROOTEventCollimator.hh"

#ifndef __ROOTBUILD__
#include "BDSCollimatorHit.hh"
#include "BDSEnergyCounterHit.hh"
#endif

#include <vector>

ClassImp(BDSOutputROOTEventCollimator)

BDSOutputROOTEventCollimator::BDSOutputROOTEventCollimator():
  primaryInteracted(false),
  primaryStopped(false),
  n(0),
  totalEnergyDeposited(0)
{;}

BDSOutputROOTEventCollimator::~BDSOutputROOTEventCollimator()
{;}

void BDSOutputROOTEventCollimator::Flush()
{
  primaryInteracted = false;
  primaryStopped    = false;
  n                 = 0;
  xIn.clear();
  yIn.clear();
  zIn.clear();
  xpIn.clear();
  ypIn.clear();
  zpIn.clear();
  T.clear();
  weight.clear();
  partID.clear();
  turnNumber.clear();
  impactParameterX.clear();
  impactParameterY.clear();
  isIon.clear();
  ionZ.clear();
  ionA.clear();
  charge.clear();
  kineticEnergy.clear();
  mass.clear();
  rigidity.clear();
  totalEnergyDeposited = 0;
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCollimator::Fill(const BDSCollimatorHit* hit)
{;}
#endif
