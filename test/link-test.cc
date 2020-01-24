/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
/**
 * @file bdsim.cc
 *
 * \mainpage
 * BDSIM © 2001-@CURRENT_YEAR@
 *
 * version @BDSIM_VERSION@
 */

#include "BDSIMLink.hh"
#include "BDSException.hh"

#include <iostream>
#include <string>
#include <vector>

int main(int /*argc*/, char** /*argv*/)
{
  BDSIMLink* bds = nullptr;
  try
    {
      bds = new BDSIMLink();
      
      std::vector<std::string> arguments = {"--file=../examples/features/link/link_collimators.gmad",
					    "--vis_debug"/*, "--batch"*/};
      std::vector<char*> argv;
      for (const auto& arg : arguments)
	{argv.push_back((char*)arg.data());}
      argv.push_back(nullptr);
     
      bds->Initialise(argv.size() - 1, argv.data()); 
      if (!bds->Initialised())
	{
	  if (bds->InitialisationResult() == 1) // if 2 it's ok
	    {std::cout << "Intialisation failed" << std::endl; return 1;}
	}
      else
	{bds->BeamOn();}
      delete bds;
    }
  catch (const BDSException& exception)
    {
      delete bds;
      std::cerr << std::endl << exception.what() << std::endl;
      exit(1);
    }
  catch (const std::exception& exception)
    {
      delete bds;
      std::cerr << std::endl << exception.what() << std::endl;
      exit(1);
    }
  return 0;
}
