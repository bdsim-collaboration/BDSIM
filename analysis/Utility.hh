#ifndef __UTILITY_H
#define __UTILITY_H

#include <vector>
#include "TH1D.h"
#include "TVector3.h"
#include "TRotation.h"


#include "Event.hh"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventHistograms.hh"

TVector3 LocalToGlobal(const TVector3 &vLocal, const TRotation &r, const TVector3 &d);
std::vector<TH1D*>& GetRun1DHistograms(TString fileName);
BDSOutputROOTEventOptions* GetOptions(TString fileName);
BDSOutputROOTEventModel* GetModel(TString fileName);
void GetEvent(TString fileName, TChain *c, Event *e);

#endif
