/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "Model.hh"
#include "RebdsimTypes.hh"

#include "BDSOutputROOTEventModel.hh"

#include "TROOT.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>

ClassImp(Model)

Model::Model():
  Model(false)
{;}

Model::Model(bool debugIn):
  model(nullptr),
  debug(debugIn)
{
  model = new BDSOutputROOTEventModel();
}

Model::~Model()
{
  delete model;
}

void Model::SetBranchAddress(TTree* t,
			     bool                      allBranchesOn,
			     const RBDS::VectorString* branchesToTurnOn)
{
  // turn off all branches by default.
  t->SetBranchStatus("*", 0);

  if (allBranchesOn)
    {t->SetBranchStatus("*", 1);}
  else if (branchesToTurnOn)
    {
      for (auto name : *branchesToTurnOn)
	{
	  std::string nameStar = name + ".*"; // necessary because of the splitting
	  if (debug)
	    {std::cout << "Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), 1);
	}
    }
  
  t->SetBranchAddress("Model.",&model);
}

std::vector<std::string> Model::SamplerNames() const
{
  if (model)
    {return model->samplerNamesUnique;}
  else
    {return std::vector<std::string>();}
}
