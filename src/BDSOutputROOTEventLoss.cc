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
#include "BDSOutputROOTEventLoss.hh"

#ifndef __ROOTBUILD__
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSEnergyCounterHit.hh"
#include "BDSTrajectoryPoint.hh"
#endif

ClassImp(BDSOutputROOTEventLoss)

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss():
  storeLinks(false),
  storeLocal(false),
  storeGlobal(false),
  storeTime(false)
{
  Flush();
}

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss(bool storeLinksIn,
					       bool storeLocalIn,
					       bool storeGlobalIn,
					       bool storeTimeIn):
  storeLinks(storeLinksIn),
  storeLocal(storeLocalIn),
  storeGlobal(storeGlobalIn),
  storeTime(storeTimeIn)
{
  Flush();
}

BDSOutputROOTEventLoss::~BDSOutputROOTEventLoss()
{;}

#ifndef __ROOTBUILD__

void BDSOutputROOTEventLoss::Fill(const BDSTrajectoryPoint* hit)
{
  n++;
  energy.push_back( (float &&) hit->GetEnergy() / CLHEP::GeV);
  S.push_back     ( (float &&) hit->GetPostS()  / CLHEP::m);
  weight.push_back( (float &&) hit->GetPostWeight());
  modelID.push_back((int &&)   hit->GetBeamLineIndex());
  turn.push_back(   (int &&)   hit->GetTurnsTaken());

  if (storeLocal)
    {
      const G4ThreeVector& pos = hit->GetPostPosLocal();
      x.push_back( (float &&) pos.x());
      y.push_back( (float &&) pos.y());
      z.push_back( (float &&) pos.z());
    }
  if (storeGlobal)
    {
      const G4ThreeVector& pos = hit->GetPosition();
      X.push_back( (float &&) pos.x());
      Y.push_back( (float &&) pos.y());
      Z.push_back( (float &&) pos.z());
    }
  if (storeTime)
    {
      T.push_back( (float &&) hit->GetPostGlobalTime() / CLHEP::ns);
    }
}
void BDSOutputROOTEventLoss::Fill(const BDSEnergyCounterHit *hit)
{
  n++;
  energy.push_back( (float &&) (hit->GetEnergy() / CLHEP::GeV));
  S.push_back     ( (float &&) (hit->GetSHit()   / CLHEP::m));
  weight.push_back( (float &&)  hit->GetWeight());

  turn.push_back( hit->GetTurnsTaken());       // TODO need to flag on circular machines

  if(storeLinks)
    {
      partID.push_back(hit->GetPartID());
      trackID.push_back(hit->GetTrackID());
      parentID.push_back(hit->GetParentID());
      modelID.push_back(hit->GetBeamlineIndex());
    }

  if(storeLocal)
    {
      x.push_back( (float &&) (hit->Getx() / CLHEP::m));
      y.push_back( (float &&) (hit->Gety() / CLHEP::m));
      z.push_back( (float &&) (hit->Getz() / CLHEP::m));
    }
  
  if(storeGlobal)
    {
      X.push_back( (float &&) (hit->GetX() / CLHEP::m));
      Y.push_back( (float &&) (hit->GetY() / CLHEP::m));
      Z.push_back( (float &&) (hit->GetZ() / CLHEP::m));
    }
  if (storeTime)
    {
      T.push_back( (float &&) hit->GetGlobalTime() / CLHEP::ns);
    }
}

#endif

void BDSOutputROOTEventLoss::Flush()
{
  n = 0;
  energy.clear();
  S.clear();
  weight.clear();
  partID.clear();
  trackID.clear();
  parentID.clear();
  modelID.clear();
  turn.clear();
  x.clear();
  y.clear();
  z.clear();
  X.clear();
  Y.clear();
  Z.clear();
  T.clear();
}
