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
  this->x.push_back((float &&) (hit->Getx() / CLHEP::m));
  this->y.push_back((float &&) (hit->Gety() / CLHEP::m));
}

void BDSOutputROOTEventHit::Flush() {
  BDSOutputROOTEventLoss::Flush();
  this->x.clear();
  this->y.clear();
}
