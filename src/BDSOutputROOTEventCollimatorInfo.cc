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
#include "BDSOutputROOTEventCollimatorInfo.hh"

#ifndef __ROOTBUILD__
#include "BDSBeamlineElement.hh"
#include "BDSTiltOffset.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#endif

ClassImp(BDSOutputROOTEventCollimatorInfo)

BDSOutputROOTEventCollimatorInfo::BDSOutputROOTEventCollimatorInfo():
  componentName(""),
  componentType(""),
  length(0),
  tilt(0),
  offsetX(0),
  offsetY(0),
  material("")
{;}

BDSOutputROOTEventCollimatorInfo::~BDSOutputROOTEventCollimatorInfo()
{;}

void BDSOutputROOTEventCollimatorInfo::Flush()
{
  componentName = "";
  componentType = "";
  length        = 0;
  tilt          = 0;
  offsetX       = 0;
  offsetY       = 0;
  material      = "";
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCollimatorInfo::Fill(const BDSBeamlineElement* element)
{
  componentName = element->GetName();
  componentType = element->GetType();
  length        = element->GetChordLength() / CLHEP::m;
  const BDSTiltOffset* to = element->GetTiltOffset();
  if (to)
    {
      tilt = to->GetTilt() / CLHEP::rad;
      offsetX = to->GetXOffset() / CLHEP::m;
      offsetY = to->GetYOffset() / CLHEP::m;
    }
  else
    {
      tilt    = 0;
      offsetX = 0;
      offsetY = 0;
    }
  material = element->GetMaterial();
}
#endif
