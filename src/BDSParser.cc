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
  options.Amalgamate(optionsIn, true);
}

void BDSParser::CheckOptions()
{
  if (options.nGenerate < 0) // run at least 1 event!
    {options.set_value("nGenerate", 1);}
  
  if (options.beamEnergy == 0)
    {
      std::cerr << __METHOD_NAME__ << "Error: option \"beam, energy\" is not defined or must be greater than 0" << std::endl;
      exit(1);
    }
  
  if (!BDS::IsFinite(options.E0))
    {options.set_value("E0", options.beamEnergy);}
}
