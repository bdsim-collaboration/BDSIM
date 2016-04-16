#ifndef __UTILITY_H
#define __UTILITY_H

#include <vector>

#include "TH1D.h"


std::vector<TH1D*>& GetRun1DHistograms(TString fileName);
BDSOutputROOTEventOptions* GetOptions(TString fileName);

#endif
