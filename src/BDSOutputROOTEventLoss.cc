#include "BDSOutputROOTEventLoss.hh"

#include "CLHEP/Units/PhysicalConstants.h"

ClassImp(BDSOutputROOTEventLoss);

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss()
{
  this->Flush();
}

BDSOutputROOTEventLoss::~BDSOutputROOTEventLoss()
{
}


void BDSOutputROOTEventLoss::Fill(BDSEnergyCounterHit *hit)
{
  this->n++;
  this->energy.push_back((float &&) (hit->GetEnergy() / CLHEP::GeV));
  this->S.push_back((float &&) (hit->GetS() / CLHEP::m));
  this->weight.push_back((float &&) hit->GetWeight());
}

void BDSOutputROOTEventLoss::Flush()
{
  this->n = 0;
  this->energy.clear();
  this->S.clear();
  this->weight.clear();
}
