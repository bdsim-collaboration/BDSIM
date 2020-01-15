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
#include "HistogramDef.hh"
#include "HistogramDefSet.hh"

#include <map>
#include <set>
#include <string>


HistogramDefSet::HistogramDefSet(const std::string&        branchNameIn,
				 const HistogramDef*       baseDefinitionIn,
				 const std::set<long long int>& pdgIDsIn,
				 bool                      dynamicallyStoreIonsIn):
  branchName(branchNameIn),
  dynamicallyStoreIons(dynamicallyStoreIonsIn),
  dynamicallyStoreParticles(pdgIDsIn.empty())
{
  if (baseDefinitionIn)
    {throw std::invalid_argument("invalid histogram definition");}

  baseDefinition = baseDefinitionIn->Clone();
  
  for (auto pdgID : pdgIDsIn)
    {
      HistogramDef* h = baseDefinitionIn->Clone();
      h->histName  = h->histName + "_" + std::to_string(pdgID);
      h->selection = setName + ".partID=="+std::to_string(pdgID);
      definitions[pdgID] = h;
    }
}

HistogramDefSet::~HistogramDefSet()
{
  delete baseDefinition;
  for (auto kv : definitions)
    {delete kv.second;}
}
