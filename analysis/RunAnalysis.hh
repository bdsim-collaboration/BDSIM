/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef RUNANALYSIS_H
#define RUNANALYSIS_H

#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"

#include "BDSOutputROOTEventHistograms.hh"

#include "Run.hh"
#include "Analysis.hh"

/**
 * @brief Analysis of a run.
 *
 * @author Stuart Walker.
 */

class RunAnalysis : public Analysis
{
public:
  RunAnalysis();
  RunAnalysis(Run *r, TChain *c, bool debug = false);
  virtual ~RunAnalysis();

  virtual void Process();

protected:
  Run* run;

  ClassDef(RunAnalysis,1);
};

#endif
