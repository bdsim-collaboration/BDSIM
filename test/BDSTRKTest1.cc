#include "parser/beam.h"
#include "parser/options.h"

#include "tracker/TRKBunch.hh"
#include "tracker/TRKLine.hh"
#include "tracker/TRKFactory.hh"
#include "tracker/TRKStrategy.hh"
#include "tracker/TRKTracker.hh"

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
#include "BDSOutputFactory.hh"
#include "BDSParser.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP

int main (int argc, char** argv)
{
  /// Initialize executable command line options reader object
  const BDSExecOptions* execOptions = new BDSExecOptions(argc,argv);
  execOptions->Print();

  /// Parse lattice file
  auto fileName = execOptions->InputFileName();
  G4cout << __FUNCTION__ << "> Using input file : "<< fileName << G4endl;
  BDSParser::Instance(fileName);

  /// Update options generated by parser with those from executable options.
  BDSParser::Instance()->AmalgamateOptions(execOptions->Options());

  // Force construction of global constants after parser has been initialised (requires materials first).
  /// This uses the options from BDSParser.
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  
  /// Local shortcut to options
  const GMAD::Options& options = BDSParser::Instance()->GetOptions();
  const GMAD::Beam&    beam    = BDSParser::Instance()->GetBeam();
  
  /// Initialise output
  BDSOutput* output = BDSOutputFactory::CreateOutput(globalConstants->OutputFormat(),
						     globalConstants->OutputFileName());

  /// Initialise random numbers
  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options or from exec options

  /// Build bunch
  TRKBunch* bunch = new TRKBunch(beam);
  /// Write primaries to output file
  output->WriteTrackerBunch("primaries",bunch,true);
  
  /// Build beamline
  TRKFactory* factory   = new TRKFactory(options, beam);
  TRKLine* beamline     = factory->CreateLine(BDSParser::Instance()->GetBeamline());
  TRKStrategy* strategy = factory->CreateStrategy();

  /// Build tracker
  TRKTracker tracker(beamline,strategy,options,output);

  //run tracking - all output through bdsim / samplers
  tracker.Track(bunch);
  
  // free memory (good code test)
  delete factory;
  delete bunch;
  delete output;

  //done
  return 0;
}
