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
#ifndef EVENTANALYSIS_H
#define EVENTANALYSIS_H

#include <vector>

#include "Analysis.hh"
#include "TROOT.h"

class Event;
class SamplerAnalysis;
class TChain;
class TFile;

/**
 * @brief Event level analysis.
 *
 * @author Stewart Boogert
 */

class EventAnalysis: public Analysis
{
public:
  EventAnalysis();
  EventAnalysis(Event*  eventIn,
		TChain* chain,
		bool    processSamplersIn   = false,
		bool    debug               = false,
		double  printModuloFraction = 0.01,
		bool    emittanceOnTheFlyIn = false);
  virtual ~EventAnalysis();

  void SetPrintModuloFraction(double fraction);

  virtual void Process();

  /// Terminate each individual sampler analysis and append optical functions.
  virtual void Terminate();

  /// Write analysis including optical functions to an output file.
  virtual void Write(TFile *outputFileName);

protected:
  Event* event;
  std::vector<SamplerAnalysis*> samplerAnalyses;
  std::vector<std::vector<std::vector<double>>> opticalFunctions; ///< optical functions from all samplers
  ClassDef(EventAnalysis,1);

private:
  /// Initialise each sampler analysis object in samplerAnalysis.
  void Initialise();
  void ProcessSamplers(bool firstTime = false);

  int  printModulo;     ///< Cache of print modulo fraction
  bool processSamplers; ///< Whether to process samplers.
  bool emittanceOnTheFly; ///< Whether to calculate emittance fresh at each sampler.
};

#endif
