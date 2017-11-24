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
#ifndef BDSIM_ANALYSISUSER_H
#define BDSIM_ANALYSISUSER_H

#include <string>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "Options.hh"
#include "Model.hh"
#include "Run.hh"
#include "Event.hh"

/**
 * @brief User analysis class
 *
 * @author Stewart Boogert
 */

class AnalysisUser
{
public:
  AnalysisUser();
  AnalysisUser(std::string filename);
  AnalysisUser(std::vector<std::string> filenames);
  void SetBranchAddresses(TTree *options, TTree *model, TTree *run, TTree *event);
  void GetEntry(int iEntry);
  void Analysis();
  virtual ~AnalysisUser();

  TFile *f           = nullptr;
  TTree *optionsTree = nullptr;
  TTree *modelTree   = nullptr;
  TTree *runTree     = nullptr;
  TTree *eventTree   = nullptr;

  Options *options   = nullptr;
  Model   *model     = nullptr;
  Event   *event     = nullptr;
  Run     *run       = nullptr;

private:
  ClassDef(AnalysisUser,1);
};


#endif
