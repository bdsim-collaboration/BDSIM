/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef PERENTRYHISTOGRAM_H
#define PERENTRYHISTOGRAM_H

#include <string>

class HistogramDef;

class TChain;
class TH1;

class PerEntryHistogram
{
public:
  explicit PerEntryHistogram(const HistogramDef* definition,
			     TChain*             chain);
  ~PerEntryHistogram(){;}

  void AccumulateCurrentEntry();

  void Terminate();

  void Write();

  TChain*       chain;        ///< Cache of chain pointer that provides data.
  unsigned int  nDimensions;  ///< Number of dimensions.
  unsigned long n;            ///< Current entry number.
  std::string   histName;     ///< Base name of histogram.
  std::string   selection;    ///< Selection command.

  TH1*          temp;         ///< Histogram for temporary 1 event data.
  TH1*          mean;         ///< Histogram for accumulated mean across events.
  TH1*          variance;     ///< Histogram for accumualted variances across events.
  TH1*          result;       ///< Final result with errors as the error on the mean.
  TH1*          resultSTD;    ///< Final result with errors as the standard devitation.
  std::string   command;      ///< Draw command.
  bool          terminated;   ///< Record of whether the instance has been terminated (single use).
  
private:
  PerEntryHistogram() = delete;

  void AccumulateSingleValue(const double&  oldMean,
			     const double&  oldVari,
			     const double&  x,
			     unsigned long& n,
			     double&        newMean,
			     double&        newVari);
};

#endif
