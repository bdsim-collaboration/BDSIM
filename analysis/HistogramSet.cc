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
#include "HistogramFactory.hh"
#include "HistogramSet.hh"

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

ClassImp(HistogramSet)

HistogramSet::HistogramSet():
  setName(""),
  definition(nullptr),
  ions(false),
  nEntries(0),
  storeAllNonIons(true),
  nDimensions(0)
{;}

HistogramSet::HistogramSet(const std::string&        setNameIn,
			   const HistogramDef*       definitionIn,
			   const std::set<long int>& pdgIDsIn,
			   bool                      ionsIn):
  setName(setNameIn),
  definition(definitionIn),
  pdgIDs(pdgIDsIn),
  ions(ionsIn),
  nEntries(0),
  storeAllNonIons(pdgIDsIn.empty()),
  nDimensions(0)
{
  if (definitionIn)
    {definition = definition->Clone();}
  else
    {throw std::invalid_argument("invalid histogram definition");}

  nDimensions = definition->nDimensions;
  HistogramFactory fac;
  for (const auto id : pdgIDs)
    {CreateHistogram(id);} 
}

HistogramSet::HistogramSet(const HistogramSet* other,
			   std::set<long long int> subsetOfPDGIDs):
  HistogramSet()
{
  if (!other)
    {throw std::invalid_argument("invalid instance to copy from");}
  setName         = other->setName;
  definition      = other->definition->Clone();
  pdgIDs          = other->pdgIDs;
  pdgIDsIons      = other->pdgIDsIons;
  nEntries        = other->nEntries;
  ions            = other->ions;
  storeAllNonIons = other->storeAllNonIons;
  for (const auto id : subsetOfPDGIDs)
    {
      if (IsIon(id))
	{
	  try // cast up from TObject to TH1 at least which is base class of all TH*
	    {binsIons[id] = static_cast<TH1*>(other->binsIons.at(id)->Clone());}
	  catch (const std::out_of_range&)
	    {throw std::invalid_argument("Invalid key for subset of HistogramSet");}
	}
      else
	{
	  try
	    {bins[id] = static_cast<TH1*>(other->bins.at(id)->Clone());}
	  catch (const std::out_of_range&)
	    {throw std::invalid_argument("Invalid key for subset of HistogramSet");}
	}
    }
}

TH1* HistogramSet::Histogram(long long int pdgID) const
{
  if (IsIon(pdgID))
    {return binsIons.at(pdgID);}
  else
    {return bins.at(pdgID);}
}

HistogramSet::~HistogramSet()
{
  delete definition;
  for (const auto id : pdgIDs)
    {delete bins[id];}
  for (const auto id : pdgIDsIons)
    {delete binsIons[id];}
}

HistogramSet* HistogramSet::TopN(int n) const
{
  std::set<long long int> keys;
  std::map<long long int, TH1*> all;
  // copy of pointers, not TH1 objects
  all.insert(bins.begin(), bins.end());
  all.insert(binsIons.begin(), binsIons.end());
  int i = 0;
  for (auto v = all.rbegin(); i < n; i++, v++)
    {keys.insert(v->first);}
  return CloneHistogramSet(keys);
}

HistogramSet* HistogramSet::Top5() const
{
  return TopN(5);
}

HistogramSet* HistogramSet::Top10() const
{
  return TopN(10);
}

HistogramSet* HistogramSet::TopNIons(int n) const
{
  std::set<long long int> keys;
  std::map<long long int, TH1*> all;
  all.insert(binsIons.begin(), binsIons.end());
  int i = 0;
  for (auto v = all.rbegin(); i < n; i++, v++)
    {keys.insert(v->first);}
  return CloneHistogramSet(keys);
}

HistogramSet* HistogramSet::Top5Ions() const
{
  return TopNIons(5);
}

HistogramSet* HistogramSet::Top10Ions() const
{
  return TopNIons(10);
}

HistogramSet* HistogramSet::Ions() const
{
  return CloneHistogramSet(pdgIDsIons);
}

TH1* HistogramSet::CreateHistogram(long long int pdgID)
{
  HistogramFactory fac;
  TH1* hist = fac.CreateHistogram(definition,
				  setName + "_" + definition->histName,
				  setName + "_" + definition->histName);
  if (IsIon(pdgID))
    {binsIons[pdgID] = hist;}
  else
    {bins[pdgID] = hist;}
  return hist;
}

void HistogramSet::Fill(int    pdgID,
			double variable[],
			double weight)
{
  nEntries += 1;
  if (pdgID == 0)
    {return;}
  TH1* hist = nullptr;
  if (IsIon(pdgID))
    {
      if (!ions)
	{return;}
      auto search = binsIons.find(pdgID);
      if (search == binsIons.end())
	{hist = CreateHistogram(pdgID);}
      else
	{hist = search->second;}
    }
  else
    {
      auto search = bins.find(pdgID);
      if (search == bins.end())
	{
	  if (!storeAllNonIons)
	    {return;}
	  hist = CreateHistogram(pdgID);
	}
      else
	{hist = search->second;}
    }
  
  switch (nDimensions)
    {
    case 1:
      {
	TH1D* h = static_cast<TH1D*>(hist);
	h->Fill(variable[0], weight);
	break;
      }
    case 2:
      {
	TH2D* h = static_cast<TH2D*>(hist);
	h->Fill(variable[0], variable[1], weight);
	break;
      }
    case 3:
      {
	TH3D* h = static_cast<TH3D*>(hist);
	h->Fill(variable[0], variable[1], variable[2], weight);
	break;
      }
    default:
      {break;}
    }
}
