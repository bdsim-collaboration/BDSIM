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
#include "TRKLine.hh"

#include <iostream>

#include "TRKElement.hh"
#include "TRKElementLine.hh"

TRKLine::TRKLine(std::string nameIn, bool circularIn) :
  name(nameIn),circular(circularIn)
{
}

void TRKLine::AddElement(TRKElement* e) {
    if (auto* ell = dynamic_cast<TRKElementLine*>(e))
    {
        for (auto &se : *ell)
        {
            AddSingleElement(se);
        }
    }
    else
    {
        AddSingleElement(e);
    }
}

void TRKLine::AddSingleElement(TRKElement* e) {
  elements.push_back(e);
  double lastS = elementEndS.empty() ? 0 : elementEndS.back();
  elementEndS.push_back(lastS + e->GetLength());
}

TRKLine::const_iterator TRKLine::FindElement(double s) const {
  auto it = std::lower_bound(elementEndS.begin(), elementEndS.end(), s);
  auto result = begin();
  std::advance(result, std::distance(elementEndS.begin(), it));
  return result;
}
