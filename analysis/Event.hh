#ifndef __EVENT_H
#define __EVENT_H

#include <vector>

#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"

#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventHistograms.hh"

#define MAXSAMPLERS 30000
class Event
{
public :
  Event();
  void CommonCtor();
  virtual ~Event();

#ifndef __ROOTDOUBLE__  
  BDSOutputROOTEventSampler<float>*     GetPrimaries();
#else
  BDSOutputROOTEventSampler<double>*    GetPrimaries();
#endif
  BDSOutputROOTEventLoss*               GetLoss();
  BDSOutputROOTEventLoss*               GetPrimaryFirstHit();
  BDSOutputROOTEventLoss*               GetPrimaryLastHit();
  BDSOutputROOTEventLoss*               GetTunnelHit();
  BDSOutputROOTEventTrajectory*         GetTrajectory();
  void SetBranchAddress(TChain *, std::vector<std::string>&);

  BDSOutputROOTEventSampler<float>               *primaries;
  BDSOutputROOTEventLoss                         *eloss;
  BDSOutputROOTEventLoss                         *primaryFirstHit;
  BDSOutputROOTEventLoss                         *primaryLastHit;
  BDSOutputROOTEventLoss                         *tunnelHit;
  BDSOutputROOTEventTrajectory                   *trajectory;
#ifndef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<float>*               samplersA[MAXSAMPLERS];
  std::vector<BDSOutputROOTEventSampler<float>*>  samplers;
#else
  BDSOutputROOTEventSampler<double>*              samplersA[MAXSAMPLERS];
  std::vector<BDSOutputROOTEventSampler<double>*> samplers;
#endif
  BDSOutputROOTEventHistograms                   *histos;
private:


  ClassDef(Event,1);

};

#endif
