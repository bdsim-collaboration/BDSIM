#include "parser/gmad.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"
#include "tracker/TRKHybrid.hh"

#include "BDSExecOptions.hh"

extern Options options;
extern ElementList beamline_list;

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
  TRKStrategy* strategy = factory->createStrategy();

  //build tracking strategy
  //int nTrackingSteps = 1;
  //TRKStrategy* strategy = new TRKHybrid(nTrackingSteps);

  //build tracker
  TRKTracker tracker(beamline,strategy);

  //run tracking - all output through bdsim
  //well will want output / transfer of particles potentially
  //after each tracking stage - will be done inside Track
  tracker.Track(bunch);
  
  //done
  return 0;
}
