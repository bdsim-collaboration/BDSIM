#include "BDSParser.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include <string>

BDSParser* BDSParser::instance = nullptr;

BDSParser* BDSParser::Instance()
{
  if(instance==nullptr) {
    std::cerr << "BDSParser has not been initialized!" << std::endl;
    exit(1);
  }
  return instance;
}

BDSParser* BDSParser::Instance(std::string name)
{
  if(instance) {
    std::cerr << "BDSParser is already initialized!" << std::endl;
    exit(1);
  }
  instance = new BDSParser(name);
  return instance;
}

BDSParser::~BDSParser()
{
  instance = nullptr;
}

BDSParser::BDSParser(std::string name):GMAD::Parser(name)
{}

void BDSParser::AmalgamateOptions(const GMAD::Options& optionsIn)
{
  if (optionsIn.recreate)
    {options.OverWrite(optionsIn);}
  else
    {options.Amalgamate(optionsIn, true);}
}

void BDSParser::CheckOptions()
{
  if (options.nGenerate < 0) // run at least 1 event!
    {options.nGenerate = 1;}
  
  if (options.beamEnergy == 0)
    {
      std::cerr << __METHOD_NAME__ << "Error: option \"beam, energy\" is not defined or must be greater than 0" << std::endl;
      exit(1);
    }
  
  if (!BDS::IsFinite(options.E0))
    {options.E0 = options.beamEnergy;}

  if(options.LPBFraction > 1.0) // safety checks
    {options.LPBFraction = 1.0;}
  if(options.LPBFraction < 0.0)
    {options.LPBFraction = 0.0;}

  if (options.lengthSafety < 1e-15)
    { // protect against poor lengthSafety choices that would cause potential overlaps
      std::cerr << "Dangerously low \"lengthSafety\" value of: " << options.lengthSafety
		<< " m that will result in potential geometry overlaps!" << std::endl;
      std::cerr << "This affects all geometry construction and should be carefully chosen!!!" << std::endl;
      std::cerr << "The default value is 1 pm" << std::endl;
      exit(1);
    }

  if(options.nturns < 1)
    {options.nturns = 1;}
}
