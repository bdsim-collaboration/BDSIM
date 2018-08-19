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
#include "colour.h"

using namespace GMAD;

Colour::Colour()
{
  clear();
  PublishMembers();
}

void Colour::clear()
{
  name  = "";
  red   = 0;
  green = 0;
  blue  = 0;
  alpha = 0;
}

void Colour::PublishMembers()
{
  publish("name",  &Colour::name);
  publish("red",   &Colour::red);
  publish("green", &Colour::green);
  publish("blue",  &Colour::blue);
  publish("alpha", &Colour::alpha);
}

void Colour::print()const
{
  std::cout << "colour: "
	    << "name "  << name  << std::endl
	    << "red "   << red   << std::endl
	    << "green " << green << std::endl
	    << "blue "  << blue  << std::endl
	    << "alpha " << alpha << std::endl;
}
