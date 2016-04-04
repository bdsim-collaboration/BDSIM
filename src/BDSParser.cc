#include "BDSParser.hh"

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
