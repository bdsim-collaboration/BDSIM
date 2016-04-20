#ifndef __UTILITY_H
#define __UTILITY_H

#include <vector>
#include "TH1D.h"
#include "Event.hh"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventHistograms.hh"

std::vector<TH1D*>& GetRun1DHistograms(TString fileName);
BDSOutputROOTEventOptions* GetOptions(TString fileName);
BDSOutputROOTEventModel* GetModel(TString fileName);
void GetEvent(TString fileName, TChain *c, Event *e);

#endif
