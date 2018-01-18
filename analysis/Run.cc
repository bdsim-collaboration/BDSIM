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
#include "RebdsimTypes.hh"
#include "Run.hh"

#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventHistograms.hh"

#include <iostream>
#include <vector>

ClassImp(Run)

Run::Run():
Run(false)
{;}

Run::Run(bool debugIn):
  info(nullptr),
  histos(nullptr),
  debug(debugIn)
{
  info   = new BDSOutputROOTEventRunInfo();
  histos = new BDSOutputROOTEventHistograms();
}

Run::~Run()
{
  delete info;
  delete histos;
}

void Run::SetBranchAddress(TTree *t,
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
	  std::string nameStar = name + "*";
	  if (debug)
	    {std::cout << "Run::SetBranchAddress> Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), 1);
	}
    }
  
  t->SetBranchAddress("Info.",&info);
  t->SetBranchAddress("Histos.",&histos);
}
