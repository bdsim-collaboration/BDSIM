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
#include <regex>
#include <set>
#include <string>

HistogramDefSet::HistogramDefSet(const std::string&  branchNameIn,
				 const HistogramDef* baseDefinitionIn,
				 const std::set<long long int>& pdgIDsIn,
				 bool                dynamicallyStoreIonsIn,
				 const std::string&  particleSpecificationIn):
  branchName(branchNameIn),
  dynamicallyStoreIons(dynamicallyStoreIonsIn),
  dynamicallyStoreParticles(pdgIDsIn.empty()),
  what(writewhat::all),
  topN(10)
{
  if (!baseDefinitionIn)
    {throw std::invalid_argument("invalid histogram definition");}

  baseDefinition = baseDefinitionIn->Clone();
  
  for (auto pdgID : pdgIDsIn)
    {
      HistogramDef* h = baseDefinitionIn->Clone();
      h->histName  = "Spectra_" + h->histName + "_" + std::to_string(pdgID);
      h->selection = AddPDGFilterToSelection(pdgID, h->selection, branchName);
      definitions[pdgID] = h;
    }

  std::string spec = particleSpecificationIn;
  std::transform(spec.begin(), spec.end(), spec.begin(), ::tolower);
  if (spec.find("all") != std::string::npos)
    {what = writewhat::all;}
  else if (spec.find("ions") != std::string::npos)
    {
      if (spec.find("topn") != std::string::npos)
	{what = writewhat::topNIons;}
      else
	{what = writewhat::ions;}
    }
  else if (spec.find("topn") != std::string::npos)
    {what = writewhat::topN;}
}

HistogramDefSet::~HistogramDefSet()
{
  delete baseDefinition;
  for (auto kv : definitions)
    {delete kv.second;}
}

std::string HistogramDefSet::AddPDGFilterToSelection(long long int      pdgID,
						     const std::string& selection,
						     const std::string& branchName)
{
  std::string filter = branchName+".partID=="+std::to_string(pdgID);
  // check if it has a boolean expression already in it
  std::string result;
  std::regex boolOperator("&&|[<>!=]=|[<>]|\\|\\|");
  std::smatch match;
  if (std::regex_search(selection, match, boolOperator))
    {// has boolean operator somewhere in it      
      std::string afterBool = match.suffix();
      std::size_t bracketPos = afterBool.find(")");
      result = selection; // copy it
      result.insert(match.position() + match.length() + bracketPos, "&&"+filter);
    }
  else if (selection.empty())
    {result = filter;}
  else
    {result = selection + "*("+filter+")";}
  return result;
}
