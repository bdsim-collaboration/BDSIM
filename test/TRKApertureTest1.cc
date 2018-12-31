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
#include "parser/gmad.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"

#include "BDSExecOptions.hh"

extern Options options;
extern ElementList beamline_list;

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());

  //build bunch
  TRKBunch* bunch       = new TRKBunch(options);
  
  //build beamline
  TRKFactory* factory   = new TRKFactory(options);
  TRKLine* beamline     = factory->createLine(beamline_list);
  TRKStrategy* strategy = factory->createStrategy();

  //build tracker
  TRKTracker tracker(beamline,strategy,options);

  //run tracking - all output through bdsim
  tracker.Track(bunch);
  
  //done
  return 0;
}
