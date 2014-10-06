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

  std::cout << *factory << std::endl;

  //build tracker
  TRKTracker tracker(beamline,strategy,options);

  std::cout << *bunch << std::endl;
  std::cout << *beamline << std::endl;

  //run tracking - all output through bdsim
  tracker.Track(bunch);
  
  //done
  return 0;
}
