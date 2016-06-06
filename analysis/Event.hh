#ifndef EVENT_H
#define EVENT_H

#include <vector>

#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"

#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"

class Event
{
public :
  Event();
  void CommonCtor();
  virtual ~Event();

#ifdef __ROOTDOUBLE__  
  BDSOutputROOTEventSampler<double>*    GetPrimaries();
#else
  BDSOutputROOTEventSampler<float>*     GetPrimaries();
#endif
  BDSOutputROOTEventLoss*               GetLoss();
  BDSOutputROOTEventLoss*               GetPrimaryFirstHit();
  BDSOutputROOTEventLoss*               GetPrimaryLastHit();
  BDSOutputROOTEventLoss*               GetTunnelHit();
  BDSOutputROOTEventTrajectory*         GetTrajectory();
  BDSOutputROOTEventHistograms*         GetHistograms();
  void SetBranchAddress(TChain *, std::vector<std::string>&);

#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>              *primaries;
#else
  BDSOutputROOTEventSampler<float>               *primaries;
#endif
  BDSOutputROOTEventLoss                         *eloss;
  BDSOutputROOTEventLoss                         *primaryFirstHit;
  BDSOutputROOTEventLoss                         *primaryLastHit;
  BDSOutputROOTEventLoss                         *tunnelHit;
  BDSOutputROOTEventTrajectory                   *trajectory;
#ifdef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<double>*> samplers;
#else
  std::vector<BDSOutputROOTEventSampler<float>*>  samplers;
#endif
  BDSOutputROOTEventHistograms                   *histos;
  BDSOutputROOTEventInfo                         *info;
private:


  ClassDef(Event,1);

};

#endif
