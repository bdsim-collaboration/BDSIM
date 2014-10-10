#include "parser/gmad.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"
#include "tracker/TRK.hh"

#include "BDSExecOptions.hh"
#include "BDSOutputBase.hh"

//Things from the parser
extern Options options;
extern ElementList beamline_list;

//GLOBALS
BDSOutputBase* trkOutput; //output interface

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());

  //initialise output
  trkOutput = TRK::InitialiseOutput();

  //build bunch
  TRKBunch* bunch       = new TRKBunch(options);
  
  //build beamline
  TRKFactory* factory   = new TRKFactory(options);
  TRKLine* beamline     = factory->createLine(beamline_list);
  TRKStrategy* strategy = factory->createStrategy();

  //build tracker
  TRKTracker tracker(beamline,strategy,options);

  //run tracking - all output through bdsim / samplers
  tracker.Track(bunch);
  
  //done
  return 0;
}
