#include "parser/gmad.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"

#include "BDSExecOptions.hh"

extern Options options;

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());

  //build bunch - DONE
  TRKBunch* bunch   = new TRKBunch(options);
  
  //build beamline
  TRKFactory* factory = new TRKFactory(options);
  TRKLine* beamline   = factory->createLine(beamline_list);

  //build tracking strategy
  TRKStrategy* strategy = new TRKStrategy();

  //build tracker
  TRKTracker tracker(beamline,strategy);

  //run tracking - all output through bdsim
  //well will want output / transfer of particles potentially
  //after each tracking stage - will be done inside Track
  tracker.Track(bunch);
  
  //done
  return 0;
}
