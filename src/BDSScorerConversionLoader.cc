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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSScorerConversionLoader.hh"

#include "globals.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4PhysicsVector.hh"
#include "G4String.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

template <class T>
BDSScorerConversionLoader<T>::BDSScorerConversionLoader()
{;}

template <class T>
BDSScorerConversionLoader<T>::~BDSScorerConversionLoader()
{;}

template <class T>
G4PhysicsVector* BDSScorerConversionLoader<T>::Load(G4String fileName)
{
  file.open(fileName);

  // test if file is valid
#ifdef USE_GZSTREAM
  bool validFile = file.rdbuf()->is_open();
#else
  bool validFile = file.is_open();
#endif
  if (!validFile)
    {throw BDSException(__METHOD_NAME__, "Invalid file name or no such file named \"" + fileName + "\"");}
  else
    {G4cout << "Scorer conversion factors - loading \"" << fileName << "\"" << G4endl;}

  G4int lineNumber = 1;
  std::string line;

  std::vector<G4double> energy;
  std::vector<G4double> conversionFactor;

  while (std::getline(file, line))
    {// read a line only if it's not a blank one

      // match a line starting with #
      std::regex comment("^\\#.*");

      // Skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
	{continue;}
      else if (std::regex_search(line, comment))
	{continue;} // skip lines starting with '#'

      std::stringstream liness(line);
      std::vector<G4double> numbers;
      G4double number;
      while (liness >> number)
	{numbers.push_back(number);}
      
      if (numbers.size() != 2)
	{
	  file.close();
	  throw BDSException(__METHOD_NAME__, "Incomplete line " + std::to_string(lineNumber));
	}
      
      energy.push_back(numbers[0]);
      conversionFactor.push_back(numbers[1]);
      
      lineNumber++;
    }
  
  file.close();
  G4PhysicsOrderedFreeVector* results = new G4PhysicsOrderedFreeVector(&energy[0], &conversionFactor[0], energy.size());
  return results;
}

template class BDSScorerConversionLoader<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSScorerConversionLoader<igzstream>;
#endif