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
#ifndef PERENTRYHISTOGRAMSET_H
#define PERENTRYHISTOGRAMSET_H
#include "HistogramDefSet.hh"
#include "PerEntryHistogram.hh"

#include "BDSOutputROOTEventSampler.hh"

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

//#include "Rtypes.h" // for classdef
//#include "TObject.h"

class Event;
class HistogramDef;
class TChain;
class TDirectory;
class TH1;

/**
 * @brief Histgram over a set of integers not number line.
 *
 * @author L. Nevay
 */

class PerEntryHistogramSet: public PerEntryHistogram
{
public:
  PerEntryHistogramSet(const HistogramDefSet* definitionIn,
		       Event*                 eventIn,
		       TChain*                chainIn);  
  virtual ~PerEntryHistogramSet();

  virtual void AccumulateCurrentEntry(long int entryNumber);
  virtual void Terminate();
  virtual void Write(TDirectory* dir = nullptr);

protected:
  ///TH1* CreateHistogram(long long int pdgID);
  inline bool IsIon(long long int pdgID) const {return pdgID > 100000000;}

  void CreatePerEntryHistogram(long long int pdgID);

  /// Utility function to find top N in set s.
  std::set<long long int> TopUtility(const std::set<long long int>& s,
				     int n) const;

  /// @{ Get top part of set.
  std::set<long long int> TopNNonIons(int n) const;
  std::set<long long int> TopNIons(int n) const;
  std::set<long long int> TopN(int n) const;
  /// @}
  
  HistogramDef* baseDefinition;
  Event*        event;
  TChain*       chain;
  std::string   branchName;
  bool          dynamicallyStoreParticles;
  bool          dynamicallyStoreIons;
  long long int nEntries;
  HistogramDefSet::writewhat what;
  int           topN;

#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* sampler;
#else
  BDSOutputROOTEventSampler<float>*  sampler;
#endif

  std::set<long long int>                     allPDGIDs;
  std::set<long long int>                     ions;
  std::set<long long int>                     nonIons;
  std::map<long long int, PerEntryHistogram*> histograms;
  std::vector<PerEntryHistogram*>             allPerEntryHistograms;

  //ClassDef(PerEntryHistogramSet, 1);
};

#endif
