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
#include "PerEntryHistogram.hh"

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

//#include "Rtypes.h" // for classdef
//#include "TObject.h"

class Event;
class HistogramDef;
class HistogramDefSet;
class TChain;
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

  /*PerEntryHistogramSet(const std::string&        setNameIn,
	       const HistogramDef*       definitionIn,
	       const std::set<long int>& pdgIDsIn = {},
	       bool                      ions     = false);*/
  //PerEntryHistogramSet(const PerEntryHistogramSet& other) = delete;
  //PerEntryHistogramSet(const PerEntryHistogramSet* other,
  //		       std::set<long long int> subsetOfPDGIDs);
  //virtual PerEntryHistogramSet* ClonePerEntryHistogramSet(const std::set<long long int>& subsetKeys = {}) const= 0;
  
  virtual void Fill(int    pdgID,
		    double variable[],
		    double weight = 1.0);

  TH1* Histogram(long long int pdgID) const;
  virtual PerEntryHistogramSet* Top5() const;
  virtual PerEntryHistogramSet* Top10() const;
  virtual PerEntryHistogramSet* Top5Ions() const;
  virtual PerEntryHistogramSet* Top10Ions() const;
  virtual PerEntryHistogramSet* Ions() const;

  PerEntryHistogramSet* TopN(int n) const;
  PerEntryHistogramSet* TopNIons(int n) const;

protected:
  TH1* CreateHistogram(long long int pdgID);
  bool IsIon(long long int pdgID) const {return pdgID > 100000000;}

  Event*                  event;
  TChain*                 chain;
  std::string             setName;
  bool                    storeIons;
  bool                    storeAllNonIons;

  long long int           nEntries;
  std::set<long int>      pdgIDs;
  std::set<long long int> pdgIDsIons;

  const HistogramDef*     definition;
  
  //unsigned int            nDimensions;

  std::map<long long int, TH1*>           bins;
  std::unordered_map<long long int, TH1*> binsIons;

  std::map<long long int, PerEntryHistogram*> particles;
  std::unordered_map<long long int, PerEntryHistogram*> ions;

  //ClassDef(PerEntryHistogramSet, 1);
};

#endif
