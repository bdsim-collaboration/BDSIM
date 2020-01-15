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
#ifndef HISTOGRAMDEFSET_H
#define HISTOGRAMDEFSET_H

#include <map>
#include <set>
#include <string>

class HistogramDef;

/**
 * @brief Specification for set of histograms.
 *
 * @author L. Nevay
 */

class HistogramDefSet
{
public:
  HistogramDefSet(const std::string&  branchNameIn,
		  const HistogramDef* baseDefinitionIn,
		  const std::set<long long int>& pdgIDsIn = {},
		  bool                dynamicallyStoreIons = false,
		  const std::string&  particleSpecificationIn = "");
  ~HistogramDefSet();

  static std::string AddPDGFilterToSelection(long long int      pdgID,
					     const std::string& selection,
					     const std::string& branchName);

  std::string   branchName;
  HistogramDef* baseDefinition;
  std::map<long long int, HistogramDef*> definitions;
  bool          dynamicallyStoreIons;
  bool          dynamicallyStoreParticles; ///< Dynamically store all non-ions.

  enum class writewhat {all, topN, ions, topNIons, nonIons, topNNonIons};
  writewhat     what;
  int           topN;  
};

#endif
