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
class TDirectory;
class TH1;

/**
 * @brief Holder for information to calculate per entry histograms.
 *
 * This creates a histogram per event and accumualtes the mean and variance
 * for every bin in a cloned histogram. This produces the mean and variance
 * per event. Minimum number of events required is 2. 
 * 
 * The algorithm used to calculate the mean and variance is one that supports
 * online calculation and is numerically stable.
 * 
 * @author Laurie Nevay
 */
class PerEntryHistogram
{
public:
  /// Constructor with a histogram definition and the chain to operate on.
  explicit PerEntryHistogram(const HistogramDef* definition,
			     TChain*             chain);
  ~PerEntryHistogram(){;}

  /// Create a histogram of the approprate dimensions for the currently loaded
  /// event then add it to the online (ie running) means and variances.
  void AccumulateCurrentEntry();

  /// Calculate the standard deviation from the online variance calculation
  /// then create an output histogram in the member result and resultSTD.
  void Terminate();

  /// Forwarding function - call Write on result histograms on the currently
  /// open file. Optional directory to specify where the histogram should be moved to.
  void Write(TDirectory* dir = nullptr);

  TChain*       chain;        ///< Cache of chain pointer that provides data.
  unsigned int  nDimensions;  ///< Number of dimensions.
  unsigned long n;            ///< Current entry number.
  std::string   histName;     ///< Base name of histogram.
  std::string   selection;    ///< Selection command.

  TH1*          temp;         ///< Histogram for temporary 1 event data.
  TH1*          mean;         ///< Histogram for accumulated mean across events.
  TH1*          variance;     ///< Histogram for accumualted variances across events.
  TH1*          result;       ///< Final result with errors as the error on the mean.
  std::string   command;      ///< Draw command.
  bool          terminated;   ///< Record of whether the instance has been terminated (single use).
  
private:
  /// No need for the default constructor.
  PerEntryHistogram() = delete;

  /// Function to accumulate for a single bin. Old mean and variance passed by const reference
  /// and new value x is added to these. Resultant values of mean and variance are written to the
  /// newMean and newVari parameters.
  void AccumulateSingleValue(const double&  oldMean,
			     const double&  oldVari,
			     const double&  x,
			     double&        newMean,
			     double&        newVari);
};

#endif
