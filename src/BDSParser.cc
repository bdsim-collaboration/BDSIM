#include "BDSParser.hh"

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

const GMAD::Options& BDSParser::GetOptions()const
{
  return options;
}

const GMAD::FastList<GMAD::Element>& BDSParser::GetBeamline()const
{
  return beamline_list;
}

const GMAD::FastList<GMAD::PhysicsBiasing>& BDSParser::GetBiasing()const
{
  return xsecbias_list;
}

const std::list<GMAD::Element>& BDSParser::GetMaterials()const
{
  return material_list;
}

const std::list<GMAD::Element>& BDSParser::GetAtoms()const
{
  return atom_list;
}

const std::vector<GMAD::Region>& BDSParser::GetRegions()const
{
  return region_list;
}

const std::vector<GMAD::CavityModel>& BDSParser::GetCavityModels()const
{
  return cavitymodel_list;
}
