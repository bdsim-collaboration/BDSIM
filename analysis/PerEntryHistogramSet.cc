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
  what(definitionIn->what),
  topN(definitionIn->topN),
  sampler(nullptr)
{
  for (const auto pdgIDDef : definitionIn->definitions)
    {
      PerEntryHistogram* hist = new PerEntryHistogram(pdgIDDef.second, chainIn);
      histograms[pdgIDDef.first] = hist;
      allPerEntryHistograms.push_back(hist); // keep vector for quick iteration each Accumualte() call
      if (IsIon(pdgIDDef.first))
	{ions.insert(pdgIDDef.first);}
      else
	{nonIons.insert(pdgIDDef.first);}
    }
  sampler = event->GetSampler(branchName); // cache sampler
}

void PerEntryHistogramSet::CreatePerEntryHistogram(long long int pdgID)
{
  allPDGIDs.insert(pdgID);
  if (IsIon(pdgID))
    {ions.insert(pdgID);}
  else
    {nonIons.insert(pdgID);}
  // copy base definition
  HistogramDef* def = baseDefinition->Clone();
  def->selection = HistogramDefSet::AddPDGFilterToSelection(pdgID,
							    def->selection,
							    branchName);

  PerEntryHistogram* hist = new PerEntryHistogram(def, chain);
  hist->AddNEmptyEntries(nEntries); // update to current number of events
  histograms[pdgID] = hist;
  allPerEntryHistograms.push_back(hist);
}

PerEntryHistogramSet::~PerEntryHistogramSet()
{
  delete baseDefinition;
  for (auto kv : allPerEntryHistograms)
    {delete kv;}
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
{
  for (auto hist : allPerEntryHistograms)
    {hist->Terminate();}
}

void PerEntryHistogramSet::Write(TDirectory* dir)
{
  if (what == HistogramDefSet::writewhat::all)
    {
      for (auto hist : allPerEntryHistograms)
	{hist->Write(dir);}
    }
  else
    {
      // form a set of desired pdgIDs
      std::set<long long int> desiredPDGIDs;
      switch (what)
	{
	case HistogramDefSet::writewhat::all:
	  {desiredPDGIDs = allPDGIDs;         break;}
	case HistogramDefSet::writewhat::topN:
	  {desiredPDGIDs = TopN(topN);        break;}
	case HistogramDefSet::writewhat::ions:
	  {desiredPDGIDs = ions;              break;}
	case HistogramDefSet::writewhat::topNIons:
	  {desiredPDGIDs = TopNIons(topN);    break;}
	case HistogramDefSet::writewhat::nonIons:
	  {desiredPDGIDs = nonIons;           break;}
	case HistogramDefSet::writewhat::topNNonIons:
	  {desiredPDGIDs = TopNNonIons(topN); break;}
	} 

      for (auto pdgID : desiredPDGIDs)
	{histograms.at(pdgID)->Write(dir);}
    }
}

std::set<long long int> PerEntryHistogramSet::TopUtility(const std::set<long long int>& s,
							 int n) const
{
  // take advantage of a set being (by definition) ordered - reverse iterate.
  std::set<long long int> result;
  int in = 0;
  for (auto i = s.rbegin(); i != s.rend() || in > n; i++, in++)
    {result.insert(*i);}
  return result;
}

std::set<long long int> PerEntryHistogramSet::TopNNonIons(int n) const
{
  return TopUtility(nonIons, n);
}

std::set<long long int> PerEntryHistogramSet::TopNIons(int n) const
{
  return TopUtility(ions, n);
}
  
std::set<long long int> PerEntryHistogramSet::TopN(int n) const
{
  return TopUtility(allPDGIDs, n);
}
