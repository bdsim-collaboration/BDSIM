/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSParser.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include <string>

BDSParser* BDSParser::instance = nullptr;

BDSParser* BDSParser::Instance()
{
  if (!instance)
    {
      std::cerr << "BDSParser has not been initialised!" << std::endl;
      exit(1);
    }
  return instance;
}

BDSParser* BDSParser::Instance(std::string name)
{
  if (instance)
    {
      std::cerr << "WARNING BDSParser was already initialised!" << std::endl;
      delete instance;
    }
  instance = new BDSParser(name);
  return instance;
}

bool BDSParser::IsInitialised()
{
  return instance ? true : false;
}

BDSParser::~BDSParser()
{
  instance = nullptr;
}

BDSParser::BDSParser(std::string name):GMAD::Parser(name)
{
  std::cout << __METHOD_NAME__ << "Using input file: "<< name << std::endl;
}

void BDSParser::AmalgamateBeam(const GMAD::Beam& execBeamIn,
			       G4bool recreate)
{
  // We only amalgamate when we're running normally. If we're recreating
  // all beam parameters will be in the output file.
  if (!recreate)
    {beam.Amalgamate(execBeamIn, true);}
}

void BDSParser::AmalgamateOptions(const GMAD::Options& optionsIn)
{
  if (optionsIn.recreate)
    {options = optionsIn;} // totally overwrite options
  else
    {options.Amalgamate(optionsIn, true);}
}

void BDSParser::CheckOptions()
{
  if (options.nGenerate <= 0) // run at least 1 event!
    {options.nGenerate = 1;}
  
  if (beam.beamEnergy <= 0)
    {
      std::cerr << __METHOD_NAME__ << "Error: option \"beam, energy\" is not defined or must be greater than 0" << std::endl;
      exit(1);
    }

  // if no explicit E0 is specified, copy the design total energy to the variable
  if (!BDS::IsFinite(beam.E0))
    {beam.E0 = beam.beamEnergy;}

  // if no different beam particle is specified, use the design particle
  if (beam.beamParticleName.empty())
    {beam.beamParticleName = beam.particle;}

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

  if(BDS::IsFinite(options.beamlineS) && beam.S0 == 0) 
    {beam.S0 = beam.S0 + options.beamlineS;}
}
