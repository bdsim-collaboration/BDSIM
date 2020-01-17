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
				 const std::string&  particleSpecificationIn):
  branchName(branchNameIn),
  dynamicallyStoreIons(false),
  dynamicallyStoreParticles(pdgIDsIn.empty()),
  what(writewhat::all),
  topN(1)
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

  if (pdgIDsIn.empty())
    {
      std::string spec = particleSpecificationIn;
      std::transform(spec.begin(), spec.end(), spec.begin(), ::tolower);
      spec = RemoveSubString(spec, "{");
      spec = RemoveSubString(spec, "}");
      std::map<std::string, writewhat> keys = {{"all",           writewhat::all},
                                               {"particles",     writewhat::particles},
                                               {"ions",          writewhat::ions},
                                               {"topn",          writewhat::topN},
                                               {"topnparticles", writewhat::topNPartcles},
                                               {"topnions",      writewhat ::topNIons}};

      auto search = keys.find(spec);
      if (search != keys.end())
        {what = search->second;}
      else
        {throw std::string("Invalid particle specifier \"" + particleSpecificationIn + "\"");}

      switch (what)
        {
          case writewhat::all:
          case writewhat::ions:
          case writewhat::topNIons:
            {dynamicallyStoreIons = true; break;}
          default:
            {break;}
        }

      switch (what)
        {
          case writewhat::all:
          case writewhat::particles:
          case writewhat::topNPartcles:
            {dynamicallyStoreParticles = true; break;}
          default:
            {break;}
        }

      switch (what)
        {
          case writewhat::topN:
          case writewhat::topNPartcles:
          case writewhat::topNIons:
            {IdentifyTopN(spec); break;}
          default:
            {break;}
        }
    }
}

int HistogramDefSet::IdentifyTopN(const std::string& word) const
{
  std::string wl = word;
  std::transform(wl.begin(), wl.end(), wl.begin(), ::tolower);

  wl = RemoveSubString(wl, "all");
  wl = RemoveSubString(wl, "ion");
  wl = RemoveSubString(wl, "top");
  wl = RemoveSubString(wl, "particles");

  int result = 1;
  if (!wl.empty())
    {
      try
        {result = std::stoi(wl);}
      catch (...)
        {result = 1;}
    }
  return result;
}

std::string HistogramDefSet::RemoveSubString(const std::string& stringIn,
                                             const std::string& wordToRemove) const
{
  std::string result = stringIn;
  while (result.find(wordToRemove) != std::string::npos)
    {
      size_t pos = result.find(wordToRemove);
      result.erase(pos, wordToRemove.size());
    }
  return result;
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
