#include "BDSOutputROOTEventLoss.hh"

#include "CLHEP/Units/PhysicalConstants.h"

ClassImp(BDSOutputROOTEventLoss)

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss() : storeLocal(false), storeGlobal(false)
{
  this->Flush();
}

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss(bool storeLocalIn, bool storeGlobalIn) :
  storeLocal(storeLocalIn), storeGlobal(storeGlobalIn)
{
  this->Flush();
}

BDSOutputROOTEventLoss::~BDSOutputROOTEventLoss()
{
}


#ifndef __ROOTBUILD__

void BDSOutputROOTEventLoss::Fill(BDSTrajectoryPoint* hit)
{
  n++;
  energy.push_back( (float &&) hit->GetEnergy() / CLHEP::GeV);
  S.push_back     ( (float &&) hit->GetPostS()   / CLHEP::m);
  weight.push_back( (float &&) hit->GetPostWeight());
  modelID.push_back( (unsigned int) hit->GetBeamLineIndex()); // TBC - the member type is just int though...
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
void BDSOutputROOTEventLoss::Fill(BDSEnergyCounterHit *hit)
{
  this->n++;
  this->energy.push_back( (float &&) (hit->GetEnergy() / CLHEP::GeV));
  this->S.push_back     ( (float &&) (hit->GetSHit()   / CLHEP::m));
  this->weight.push_back( (float &&)  hit->GetWeight());
  this->modelID.push_back( (unsigned int)hit->GetBeamlineIndex());
  this->turn.push_back( hit->GetTurnsTaken());

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

void BDSOutputROOTEventLoss::Fill(BDSTunnelHit *hit)
{
  this->n++;
  this->energy.push_back( (float &&) (hit->GetEnergy() / CLHEP::GeV));
  this->S.push_back     ( (float &&)      (hit->GetS() / CLHEP::m));
  this->weight.push_back( (float &&) hit->GetWeight());
  this->turn.push_back( hit->GetTurnsTaken());

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
  this->modelID.clear();
  this->turn.clear();
  this->x.clear();
  this->y.clear();
  this->z.clear();
  this->X.clear();
  this->Y.clear();
  this->Z.clear();


  // this->geomFlag.clear();
}
