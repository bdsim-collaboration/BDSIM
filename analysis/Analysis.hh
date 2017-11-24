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
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include "HistogramMerge.hh"

#include <map>
#include <string>
#include <vector>

class HistogramDef;
class TChain;
class TFile;

/**
 * @brief Base class for any TTree analysis.
 *
 * @author Stuart Walker.
 */

class Analysis
{
public:
  /// Requires the name of the tree to analysed, the chain of files to operate on,
  /// the name of the directory to create in the output root file for the combined
  /// existing histrograms from that tree and whether or not we're in debug mode.
  Analysis(std::string treeNameIn,
	   TChain*     chainIn,
	   std::string mergedHistogramNameIn,
	   bool        debugIn = false);
  virtual ~Analysis();

  /// Method which calls all other methods.
  virtual void Execute();
  virtual void Process() = 0;

  /// Virtual function for user to overload and use. Does nothing by default.
  virtual void UserProcess();
  /// Process histogram definitions from configuration instance.
  virtual void SimpleHistograms();
  virtual void Terminate();

  /// Write rebdsim histograms.
  virtual void Write(TFile* outputFile);

protected:
  std::string treeName;
  TChain*     chain;
  std::string                 mergedHistogramName; ///< Name of directory for merged histograms
  std::vector<std::string>    histogramNames; ///< Rebdsim generated histogram names
  std::map<std::string, TH1*> histograms1D;   ///< Rebdsim 1d histogram
  std::map<std::string, TH2*> histograms2D;   ///< Rebdsim 2d histograms
  std::map<std::string, TH3*> histograms3D;   ///< Rebdsim 3d histograms
  HistogramMerge*             histoSum;       ///< Bdsim histograms
  bool                        debug;
  
private:
  Analysis() = delete;
  
  /// Create an individual histogram based on a definition.
  void FillHistogram(HistogramDef* definition);

};

#endif
