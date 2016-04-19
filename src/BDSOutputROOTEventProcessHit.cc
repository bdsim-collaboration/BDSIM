#include "BDSOutputROOTEventProcessHit.hh"

#include "CLHEP/Units/PhysicalConstants.h"

ClassImp(BDSOutputROOTEventProcessHit)

BDSOutputROOTEventProcessHit::BDSOutputROOTEventProcessHit() : BDSOutputROOTEventHit()
{
  this->Flush();
}

BDSOutputROOTEventProcessHit::~BDSOutputROOTEventProcessHit()
{
}


#ifndef __ROOTBUILD__
void BDSOutputROOTEventProcessHit::Fill(BDSEnergyCounterHit *hit)
{
  // Fill base class
  BDSOutputROOTEventHit::Fill(hit);

}

#endif

void BDSOutputROOTEventProcessHit::Flush() {
  BDSOutputROOTEventHit::Flush();

  process.clear();
  subprocess.clear();
  track.clear();
}
