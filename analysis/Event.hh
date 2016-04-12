#ifndef __EVENT_H
#define __EVENT_H

#include <vector>

#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"

#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventHit.hh"
#include "BDSOutputROOTEventTrajectory.hh"

#define MAXSAMPLERS 30000
class Event
{
public :
  Event();
  void CommonCtor();
  virtual ~Event();
  
  BDSOutputROOTEventSampler<float>*    GetPrimaries();
  BDSOutputROOTEventLoss*       GetLoss();
  BDSOutputROOTEventHit*        GetPrimaryFirstHit();
  BDSOutputROOTEventHit*        GetPrimaryLastHit();
  BDSOutputROOTEventHit*        GetTunnelHit();
  BDSOutputROOTEventTrajectory* GetTrajectory();
  void SetBranchAddress(TChain *, std::vector<std::string>&);

  BDSOutputROOTEventSampler<float>        *primaries;
  BDSOutputROOTEventLoss                  *eloss;
  BDSOutputROOTEventHit                   *primaryFirstHit;
  BDSOutputROOTEventHit                   *primaryLastHit;
  BDSOutputROOTEventHit                   *tunnelHit;
  BDSOutputROOTEventTrajectory            *trajectory;
  BDSOutputROOTEventSampler<float>*       samplersA[MAXSAMPLERS];
  std::vector<BDSOutputROOTEventSampler<float>*> samplers;
private:


  ClassDef(Event,1);

};

#endif
