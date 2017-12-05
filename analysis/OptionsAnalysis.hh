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
#ifndef OPTIONSANALYSIS_H
#define OPTIONSANALYSIS_H

#include "TROOT.h"

#include "Analysis.hh"

class Options;
class TChain;

/**
 * @brief Analysis of the options tree.
 * 
 * @author Stuart Walker.
 */

class OptionsAnalysis : public Analysis
{
public:
  OptionsAnalysis();
  OptionsAnalysis(Options* optionsIn, TChain* chain, bool debug = false);
  virtual ~OptionsAnalysis();

  virtual void Process(){};
  virtual void Terminate(){};

protected:
  Options* options;
  ClassDef(OptionsAnalysis,1);
};

#endif
