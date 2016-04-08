#include <vector>

#include "TROOT.h"
#include "TTree.h"

#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventHit.hh"
#include "BDSOutputROOTEventTrajectory.hh"

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
  void SetBranchAddress(TTree *);

private:
  BDSOutputROOTEventSampler               *primaries;
  BDSOutputROOTEventLoss                  *eloss;
  BDSOutputROOTEventHit                   *primaryFirstHit;
  BDSOutputROOTEventHit                   *primaryLastHit;
  BDSOutputROOTEventHit                   *tunnelHit;
  BDSOutputROOTEventTrajectory            *trajectory;
  std::vector<BDSOutputROOTEventSampler*> samplers;

  ClassDef(Event,1);

};
