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
#include "BDSOutputROOTEventCollimator.hh"

#ifndef __ROOTBUILD__
//#include "BDSParticleCoords.hh"
#endif

ClassImp(BDSOutputROOTEventCollimator)

BDSOutputROOTEventCollimator::BDSOutputROOTEventCollimator():
  interacted(false),
  primaryStopped(false),
  xIn(0),
  yIn(0),
  zIn(0),
  SIn(0),
  energyIn(0),
  xOut(0),
  yOut(0),
  zOut(0),
  SOut(0),
  energyDeposited(0),
  impactParameterX(0),
  impactParameterY(0),
  n(0)
{;}

BDSOutputROOTEventCollimator::~BDSOutputROOTEventCollimator()
{;}

void BDSOutputROOTEventCollimator::Flush()
{
  interacted = false;
  primaryStopped = false;
  xIn = 0;
  yIn = 0;
  zIn = 0;
  SIn = 0;
  energyIn = 0;
  xOut = 0;
  yOut = 0;
  zOut = 0;
  SOut = 0;
  energyDeposited = 0;
  impactParameterX = 0;
  impactParameterY = 0;
  n = 0;
  energy.clear();
  weight.clear();
  partID.clear();
  z.clear();
  stepLength.clear();
  x.clear();
  y.clear();
  T.clear();
  turn.clear();
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCollimator::Fill()
{;}
#endif
