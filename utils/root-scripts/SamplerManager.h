/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef SamplerManager_h
#define SamplerManager_h


#include <vector> 
#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TKey.h"
#include "TChain.h"

#include "Sampler.h"

class SamplerManager {
 public : 
  SamplerManager();
  SamplerManager(TString fileName);
  ~SamplerManager();

  void AddFile(TString fileName);
  void Init();
  void PrintTrees(); 
  void PrintFiles();
  void MakeSingleSimpleTree(); 

 private:
  std::vector<TString>  fileNames;
  std::vector<TString>  treeNames; 
  std::vector<TFile*>   files;
  std::vector<TChain*>  chains;
  std::vector<Sampler*> samplers;
};


#endif
