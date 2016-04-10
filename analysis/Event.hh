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
  
  BDSOutputROOTEventSampler*    GetPrimaries();
  BDSOutputROOTEventLoss*       GetLoss();
  BDSOutputROOTEventHit*        GetPrimaryFirstHit();
  BDSOutputROOTEventHit*        GetPrimaryLastHit();
  BDSOutputROOTEventHit*        GetTunnelHit();
  BDSOutputROOTEventTrajectory* GetTrajectory();
  void SetBranchAddress(TChain *, std::vector<std::string>&);

  BDSOutputROOTEventSampler               *primaries;
  BDSOutputROOTEventLoss                  *eloss;
  BDSOutputROOTEventHit                   *primaryFirstHit;
  BDSOutputROOTEventHit                   *primaryLastHit;
  BDSOutputROOTEventHit                   *tunnelHit;
  BDSOutputROOTEventTrajectory            *trajectory;
  BDSOutputROOTEventSampler*              samplersA[MAXSAMPLERS];
  std::vector<BDSOutputROOTEventSampler*> samplers;
private:


  ClassDef(Event,1);

};

#endif