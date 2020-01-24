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

int main(int argc, char** argv)
{
  BDSIMLink* bds = nullptr;
  try
    {
      bds = new BDSIMLink(argc, argv);
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
