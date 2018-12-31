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
#include "BDSImportanceFileLoader.hh"

#include "globals.hh" // geant4 types / globals

#include <algorithm>
#include <fstream>
#include <iostream>
#include <exception>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "globals.hh"
#include "G4String.hh"

#ifdef USE_GZSTREAM
#include "gzstream.h"
#endif

BDSImportanceFileLoader* BDSImportanceFileLoader::instance = nullptr;

BDSImportanceFileLoader* BDSImportanceFileLoader::Instance()
{
  if (!instance)
    {instance = new BDSImportanceFileLoader();}
  return instance;
}

BDSImportanceFileLoader::BDSImportanceFileLoader()
{;}

BDSImportanceFileLoader::~BDSImportanceFileLoader()
{instance = nullptr;}


std::map<G4String, G4double> BDSImportanceFileLoader::Load(G4String fileName)
{
  std::ifstream file;

  file.open(fileName);


  // test if file is valid
#ifdef USE_GZSTREAM
  bool validFile = file.rdbuf()->is_open();
#else
  bool validFile = file.is_open();
#endif

  if (!validFile)
    {
      G4cerr << "Invalid file name or no such file named \"" << fileName << "\"" << G4endl;
      exit(1);
    }
  else
    {G4cout << "BDSIM importance file - loading \"" << fileName << "\"" << G4endl;}

  std::string line;

  std::map<G4String, G4double> importance;

  while (std::getline(file, line))
    { // read a line only if it's not a blank one

      std::istringstream liness(line);
      std::string volume;
      G4double importanceValue;

      // Skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
        {continue;}

      liness >> volume >> importanceValue;
      volumes.push_back(volume);
      importanceValues.push_back(importanceValue);
      importance[volume] = importanceValue;
    }

  file.close();

  return importance;
}
