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
#ifndef HISTOGRAMSET_H
#define HISTOGRAMSET_H

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Rtypes.h" // for classdef
#include "TObject.h"

class HistogramDef;
class TH1;

/**
 * @brief Histgram over a set of integers not number line.
 *
 * @author L. Nevay
 */

class HistogramSet: public TObject
{
public:
  /// Public constructor only for compatibility with ROOT - not indended for use.
  HistogramSet();

  /// Use this constructor.
  HistogramSet(const std::string&        setNameIn,
	       const HistogramDef*       definitionIn,
	       const std::set<long int>& pdgIDsIn = {},
	       bool                      ions     = false);

  HistogramSet(const HistogramSet& other) = delete;
  HistogramSet(const HistogramSet* other,
	       std::set<long long int> subsetOfPDGIDs);
  virtual ~HistogramSet();

  virtual HistogramSet* CloneHistogramSet(const std::set<long long int>& subsetKeys = {}) const= 0;
  
  virtual void Fill(int    pdgID,
		    double variable[],
		    double weight = 1.0);

  TH1* Histogram(long long int pdgID) const;
  virtual HistogramSet* Top5() const;
  virtual HistogramSet* Top10() const;
  virtual HistogramSet* Top5Ions() const;
  virtual HistogramSet* Top10Ions() const;
  virtual HistogramSet* Ions() const;

  HistogramSet* TopN(int n) const;
  HistogramSet* TopNIons(int n) const;

protected:
  TH1* CreateHistogram(long long int pdgID);
  bool IsIon(long long int pdgID) const {return pdgID > 100000000;}
  
  std::string             setName;
  const HistogramDef*     definition;
  std::set<long int>      pdgIDs;
  std::set<long long int> pdgIDsIons;
  bool                    ions;
  long long int           nEntries;
  bool                    storeAllNonIons;
  unsigned int            nDimensions;

  std::map<long long int, TH1*>           bins;
  std::unordered_map<long long int, TH1*> binsIons;

  ClassDef(HistogramSet, 1);
};

#endif
