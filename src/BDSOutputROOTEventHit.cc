#include "BDSOutputROOTEventHit.hh"

#include "CLHEP/Units/PhysicalConstants.h"

ClassImp(BDSOutputROOTEventHit);

BDSOutputROOTEventHit::BDSOutputROOTEventHit() : BDSOutputROOTEventLoss()
{
  this->Flush();
}


BDSOutputROOTEventHit::~BDSOutputROOTEventHit()
{
}


void BDSOutputROOTEventHit::Fill(BDSEnergyCounterHit *hit)
{
  // Fill base class
  BDSOutputROOTEventLoss::Fill(hit);
  this->x.push_back( (float &&) (hit->Getx() / CLHEP::m));
  this->y.push_back( (float &&) (hit->Gety() / CLHEP::m));
  this->z.push_back( (float &&) (hit->Getz() / CLHEP::m));
}

void BDSOutputROOTEventHit::Fill(BDSTunnelHit *hit) // TODO Could we tidy the structure of BDSTunnelHit to be more similar to BDSEnergyCounterHit
{
  // Fill base class
  BDSOutputROOTEventLoss::Fill(hit);
  this->x.push_back((float &&) (hit->Getx() / CLHEP::m));
  this->y.push_back((float &&) (hit->Gety() / CLHEP::m));
  this->X.push_back((float &&) (hit->GetX() / CLHEP::m));
  this->Y.push_back((float &&) (hit->GetY() / CLHEP::m));
  this->Z.push_back((float &&) (hit->GetZ() / CLHEP::m));
}


void BDSOutputROOTEventHit::Flush() {
  BDSOutputROOTEventLoss::Flush();
  this->x.clear();
  this->y.clear();
  this->z.clear();
  this->X.clear();
  this->Y.clear();
  this->Z.clear();
}
