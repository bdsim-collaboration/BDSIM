#include "parser/gmad.h"
#include "parser/options.h"
#include "parser/fastlist.h"
#include "parser/element.h"

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
namespace GMAD
{
  extern FastList<Element> beamline_list;
  extern Options options;
}

//GLOBALS
BDSOutputBase* trkOutput=NULL; //output interface

int main (int argc, char** argv){
  //for now, need exec options parsing from bdsim
  BDSExecOptions::Instance(argc,argv)->Print();
  
  //parse input
  GMAD::gmad_parser(BDSExecOptions::Instance()->GetInputFilename());
  
  //initialise output
  //trkOutput = BDSOutputFactory::Instance()->InitialiseOutput();
  trkOutput = BDSOutputFactory::CreateOutput(BDSExecOptions::Instance()->GetOutputFormat());

  //initialise random numbers
  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options or from exec options
  if (BDSExecOptions::Instance()->SetSeedState()) //optionally load the seed state from file
    {BDSRandom::LoadSeedState(BDSExecOptions::Instance()->GetSeedStateFilename());}
  BDSRandom::WriteSeedState(); //write the current state once set / loaded

  //build bunch
  TRKBunch* bunch       = new TRKBunch(GMAD::options);
  
  //build beamline
  TRKFactory* factory   = new TRKFactory(GMAD::options);
  TRKLine* beamline     = factory->createLine(GMAD::beamline_list);
  TRKStrategy* strategy = factory->createStrategy();

  //build tracker
  TRKTracker tracker(beamline,strategy,GMAD::options);

  //run tracking - all output through bdsim / samplers
  tracker.Track(bunch);
  
  // free memory (good code test)
  delete factory;
  delete bunch;

  //done
  return 0;
}
