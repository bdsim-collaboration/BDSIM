/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "TRKLine.hh"

#include <iostream>

#include "TRKElement.hh"

TRKLine::TRKLine(std::string nameIn, bool circularIn) :
  name(nameIn),circular(circularIn)
{
}

TRKLine::~TRKLine()
{
}

void TRKLine::AddElement(TRKElement* e) {
  elements.push_back(e);
}

TRKElement* TRKLine::FindElement(std::string eName)const {
 TRKLineConstIter elIter = elements.begin();
 TRKLineConstIter elIterEnd = elements.end();
  for (;elIter!=elIterEnd; ++elIter) {
    if ((*elIter)->GetName() == eName) {
      return (*elIter);
    }
  }
  /// if not found return NULL
  return NULL;
}

/// output stream
std::ostream& operator<< (std::ostream &out, const TRKLine &line) {
  TRKLineConstIter elIter = line.begin();
  TRKLineConstIter elIterEnd = line.end();
  for (;elIter!=elIterEnd; ++elIter) {
    out << **elIter << std::endl;
  }
  return out;
}
