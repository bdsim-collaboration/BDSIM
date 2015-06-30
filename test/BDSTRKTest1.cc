#include "parser/gmad.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"

#include "BDSExecOptions.hh"
#include "BDSOutputBase.hh"
#include "BDSOutputFactory.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP

//Things from the parser
extern Options options;
extern ElementList beamline_list;

//GLOBALS
BDSOutputBase* trkOutput=NULL; //output interface

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());
  
  //initialise output
  //trkOutput = BDSOutputFactory::Instance()->InitialiseOutput();
  trkOutput = BDSOutputFactory::createOutput(BDSExecOptions::Instance()->GetOutputFormat());

  //initialise random numbers
  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options or from exec options
  if (BDSExecOptions::Instance()->SetSeedState()) //optionally load the seed state from file
    {BDSRandom::LoadSeedState(BDSExecOptions::Instance()->GetSeedStateFilename());}
  BDSRandom::WriteSeedState(); //write the current state once set / loaded

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
  
  // free memory (good code test)
  delete factory;
  delete bunch;

  //done
  return 0;
}
