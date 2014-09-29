#include "parser/gmad.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"
#include "tracker/TRKHybrid.hh"

#include "BDSExecOptions.hh"

#include <iostream>

extern Options options;
extern ElementList beamline_list;

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());

  //build bunch - DONE
  TRKBunch* bunch   = new TRKBunch(options);

  std::cout << *bunch << std::endl;
  
  
  return 0;
}
