#ifndef BDSOUTPUTROOTEVENTPROCESSHIT_H
#define BDSOUTPUTROOTEVENTPROCESSHIT_H

#include "BDSOutputROOTEventHit.hh"

#ifndef __ROOTBUILD__
#include "BDSEnergyCounterHit.hh"
#include "BDSTunnelHit.hh"
#endif


class BDSOutputROOTEventProcessHit : public BDSOutputROOTEventHit {

public:
  /* local coordinates */
  std::vector<int> process;
  std::vector<int> subprocess;
  std::vector<int> track;

  BDSOutputROOTEventProcessHit();
  virtual ~BDSOutputROOTEventProcessHit();
#ifndef __ROOTBUILD__
  virtual void Fill(BDSEnergyCounterHit *hit);
#endif
  virtual void Flush();

ClassDef(BDSOutputROOTEventProcessHit,1);
};

#endif
