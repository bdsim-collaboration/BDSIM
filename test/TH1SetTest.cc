/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "TFile.h"
#include "TTree.h"

#include "TH1Set.hh"


int main(int /*argc*/, char** /*argv*/)
{

  TFile* tf = new TFile("t1.root");
  TTree* event = (TTree*)tf->Get("Event");

  TH1Set* hist = new TH1Set("test1", "test1");
  event->Draw("c1.partID >> test1", "", "goff");
  hist->Dump();
  tf->Close();
  return 0;
}
