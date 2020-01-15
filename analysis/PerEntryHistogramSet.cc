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
#include "Event.hh"
#include "HistogramDef.hh"
#include "HistogramDefSet.hh"
#include "PerEntryHistogram.hh"
#include "PerEntryHistogramSet.hh"

#include "BDSOutputROOTEventSampler.hh"

#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

//ClassImp(PerEntryHistogramSet)

PerEntryHistogramSet::PerEntryHistogramSet(const HistogramDefSet* definitionIn,
					   Event*                 eventIn,
					   TChain*                chainIn):
  baseDefinition(definitionIn->baseDefinition),
  event(eventIn),
  chain(chainIn),
  branchName(definitionIn->branchName),
  dynamicallyStoreParticles(definitionIn->dynamicallyStoreParticles),
  dynamicallyStoreIons(definitionIn->dynamicallyStoreIons),
  nEntries(0),
  sampler(nullptr)
{
  for (const auto pdgIDDef : definitionIn->definitions)
    {
      PerEntryHistogram* hist = new PerEntryHistogram(pdgIDDef.second, chainIn);
      if (IsIon(pdgIDDef.first))
        {ions[pdgIDDef.first] = hist;}
      else
        {particles[pdgIDDef.first] = hist;}
      allPerEntryHistograms.push_back(hist); // keep vector for quick iteration each Accumualte() call
    }
  sampler = event->GetSampler(branchName); // cache sampler
}

void PerEntryHistogramSet::CreatePerEntryHistogram(long long int pdgID)
{
  // copy base definition
  HistogramDef* def = baseDefinition->Clone();
  def->selection = HistogramDefSet::AddPDGFilterToSelection(pdgID,
							    def->selection,
							    branchName);

  PerEntryHistogram* hist = new PerEntryHistogram(def, chain);
  hist->AddNEmptyEntries(nEntries); // update to current number of events
  if (IsIon(pdgID))
    {ions[pdgID] = hist;}
  else
    {particles[pdgID] = hist;}
  allPerEntryHistograms.push_back(hist);
}

PerEntryHistogramSet::~PerEntryHistogramSet()
{
  for (auto kv : particles)
    {delete kv.second;}
  for (auto kv : ions)
    {delete kv.second;}
}

void PerEntryHistogramSet::AccumulateCurrentEntry(long int entryNumber)
{
  nEntries += 1;

  if (dynamicallyStoreParticles || dynamicallyStoreIons)
    {
      // for this event, loop over all pdgIDs and make a set of them
      // then ensure we have all prepare histograms
      const auto &partID = sampler->partID;
      std::set<long long int> pdgIDSet;
      for (const auto id : partID)
        {pdgIDSet.insert(id);}

      // use vector because set cannot be appended to for set_difference
      std::vector<long long int> missing;
      std::set_difference(pdgIDSet.begin(), pdgIDSet.end(),
          allPDGIDs.begin(), allPDGIDs.end(),
          std::back_inserter(missing));
      if (!missing.empty())
        {
          for (auto pdgID : missing)
            {
              bool isIon = IsIon(pdgID);
              if ((isIon && dynamicallyStoreIons) || (!isIon && dynamicallyStoreParticles))
                {CreatePerEntryHistogram(pdgID);}
            }
        }
    }

  for (auto hist : allPerEntryHistograms)
    {hist->AccumulateCurrentEntry(entryNumber);}
}

void PerEntryHistogramSet::Terminate()
{}

void PerEntryHistogramSet::Write(TDirectory* dir)
{

}

/*
PerEntryHistogramSet::PerEntryHistogramSet(const std::string&        branchNameIn,
			   const HistogramDef*       definitionIn,
			   const std::set<long int>& pdgIDsIn,
			   bool                      ionsIn):
  branchName(branchNameIn),
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

PerEntryHistogramSet::PerEntryHistogramSet(const PerEntryHistogramSet* other,
			   std::set<long long int> subsetOfPDGIDs):
  PerEntryHistogramSet()
{
  if (!other)
    {throw std::invalid_argument("invalid instance to copy from");}
  branchName         = other->branchName;
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
	    {throw std::invalid_argument("Invalid key for subset of PerEntryHistogramSet");}
	}
      else
	{
	  try
	    {bins[id] = static_cast<TH1*>(other->bins.at(id)->Clone());}
	  catch (const std::out_of_range&)
	    {throw std::invalid_argument("Invalid key for subset of PerEntryHistogramSet");}
	}
    }
}

TH1* PerEntryHistogramSet::Histogram(long long int pdgID) const
{
  if (IsIon(pdgID))
    {return binsIons.at(pdgID);}
  else
    {return bins.at(pdgID);}
}

PerEntryHistogramSet::~PerEntryHistogramSet()
{
  delete definition;
  for (const auto id : pdgIDs)
    {delete bins[id];}
  for (const auto id : pdgIDsIons)
    {delete binsIons[id];}
}

PerEntryHistogramSet* PerEntryHistogramSet::TopN(int n) const
{
  std::set<long long int> keys;
  std::map<long long int, TH1*> all;
  // copy of pointers, not TH1 objects
  all.insert(bins.begin(), bins.end());
  all.insert(binsIons.begin(), binsIons.end());
  int i = 0;
  for (auto v = all.rbegin(); i < n; i++, v++)
    {keys.insert(v->first);}
  return ClonePerEntryHistogramSet(keys);
}

PerEntryHistogramSet* PerEntryHistogramSet::Top5() const
{
  return TopN(5);
}

PerEntryHistogramSet* PerEntryHistogramSet::Top10() const
{
  return TopN(10);
}

PerEntryHistogramSet* PerEntryHistogramSet::TopNIons(int n) const
{
  std::set<long long int> keys;
  std::map<long long int, TH1*> all;
  all.insert(binsIons.begin(), binsIons.end());
  int i = 0;
  for (auto v = all.rbegin(); i < n; i++, v++)
    {keys.insert(v->first);}
  return ClonePerEntryHistogramSet(keys);
}

PerEntryHistogramSet* PerEntryHistogramSet::Top5Ions() const
{
  return TopNIons(5);
}

PerEntryHistogramSet* PerEntryHistogramSet::Top10Ions() const
{
  return TopNIons(10);
}

PerEntryHistogramSet* PerEntryHistogramSet::Ions() const
{
  return ClonePerEntryHistogramSet(pdgIDsIons);
}

TH1* PerEntryHistogramSet::CreateHistogram(long long int pdgID)
{
  HistogramFactory fac;
  TH1* hist = fac.CreateHistogram(definition,
				  branchName + "_" + definition->histName,
				  branchName + "_" + definition->histName);
  if (IsIon(pdgID))
    {binsIons[pdgID] = hist;}
  else
    {bins[pdgID] = hist;}
  return hist;
}

void PerEntryHistogramSet::Fill(int    pdgID,
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
*/
