/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSIMClass.hh"

#include <iostream>

int main(int argc, char** argv)
{
  BDSIM* bds = new BDSIM(argc, argv);
  if (!bds->Initialised())
    {std::cout << "Intialisation failed" << std::endl; return 1;}

  std::cout << "Custom stuff here" << std::endl;
  
  bds->BeamOn();
  delete bds;
  return 0;
}
