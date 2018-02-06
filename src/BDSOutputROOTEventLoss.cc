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

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss() : storeLinks(false), storeLocal(false), storeGlobal(false)
{
  this->Flush();
}

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss(bool storeLinksIn, bool storeLocalIn, bool storeGlobalIn) :
  storeLinks(storeLinksIn), storeLocal(storeLocalIn), storeGlobal(storeGlobalIn)
{
  this->Flush();
}

BDSOutputROOTEventLoss::~BDSOutputROOTEventLoss()
{
}


#ifndef __ROOTBUILD__

void BDSOutputROOTEventLoss::Fill(const BDSTrajectoryPoint* hit)
{
  n++;
  energy.push_back( (float &&) hit->GetEnergy() / CLHEP::GeV);
  S.push_back     ( (float &&) hit->GetPostS()   / CLHEP::m);
  weight.push_back( (float &&) hit->GetPostWeight());
  modelID.push_back( hit->GetBeamLineIndex()); // TBC - the member type is just int though...
  turn.push_back( (int) hit->GetTurnsTaken());

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

}
void BDSOutputROOTEventLoss::Fill(const BDSEnergyCounterHit *hit)
{
  this->n++;
  this->energy.push_back( (float &&) (hit->GetEnergy() / CLHEP::GeV));
  this->S.push_back     ( (float &&) (hit->GetSHit()   / CLHEP::m));
  this->weight.push_back( (float &&)  hit->GetWeight());

  this->turn.push_back( hit->GetTurnsTaken());       // TODO need to flag on circular machines

  if(this->storeLinks) {
    this->partID.push_back(hit->GetPartID());
    this->trackID.push_back(hit->GetTrackID());
    this->parentID.push_back(hit->GetParentID());
    this->modelID.push_back(hit->GetBeamlineIndex());
  }

  if(this->storeLocal) {
    this->x.push_back( (float &&) (hit->Getx() / CLHEP::m));
    this->y.push_back( (float &&) (hit->Gety() / CLHEP::m));
    this->z.push_back( (float &&) (hit->Getz() / CLHEP::m));
  }

  if(this->storeGlobal) {
    this->X.push_back( (float &&) (hit->GetX() / CLHEP::m));
    this->Y.push_back( (float &&) (hit->GetY() / CLHEP::m));
    this->Z.push_back( (float &&) (hit->GetZ() / CLHEP::m));
  }
}

#endif

void BDSOutputROOTEventLoss::Flush()
{
  this->n = 0;
  this->energy.clear();
  this->S.clear();
  this->weight.clear();
  this->partID.clear();
  this->trackID.clear();
  this->parentID.clear();
  this->modelID.clear();
  this->turn.clear();
  this->x.clear();
  this->y.clear();
  this->z.clear();
  this->X.clear();
  this->Y.clear();
  this->Z.clear();
}
