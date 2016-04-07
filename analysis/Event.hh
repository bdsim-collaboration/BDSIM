#include <vector>

#include "TROOT.h"

#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventHit.hh"
#include "BDSOutputROOTEventTrajectory.hh"

class Event
{
public :
  Event();
  virtual ~Event();
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