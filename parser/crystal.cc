/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "crystal.h"

using namespace GMAD;

Crystal::Crystal()
{
  clear();
  PublishMembers();
}

void Crystal::clear()
{
  name              = "";
  material          = "";
  data              = "";
  shape             = "";
  lengthX           = 0;
  lengthY           = 0;
  lengthZ           = 0;
  bendingAngleYAxis = 0;
  bendingAngleZAxis = 0;
}

void Crystal::PublishMembers()
{
  publish("name",      &Crystal::name);
  publish("material",  &Crystal::material);
  publish("data",      &Crystal::data);
  publish("shape",     &Crystal::shape);
  publish("lengthX",   &Crystal::lengthX);
  publish("lengthY",   &Crystal::lengthY);
  publish("lengthZ",   &Crystal::lengthZ);
  publish("bendingAngleYAxis", &Crystal::bendingAngleYAxis);
  publish("bendingAngleZAxis", &Crystal::bendingAngleZAxis);
}

void Crystal::print()const
{
  std::cout << "crystal: "
	    << "name "     << name     << std::endl
	    << "material " << material << std::endl
	    << "data "     << data     << std::endl
	    << "shape "    << shape    << std::endl
	    << "lengthX "  << lengthX  << std::endl
    	    << "lengthY "  << lengthY  << std::endl
    	    << "lengthZ "  << lengthZ  << std::endl
	    << "bendingAngleYAxis " << bendingAngleYAxis << std::endl
	    << "bendingAngleZAxis " << bendingAngleZAxis << std::endl;
}
