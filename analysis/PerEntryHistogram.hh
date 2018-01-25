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

class TH1D;

class PerEntryHistogram
{
public:
  PerEntryHistogram(const HistogramDef* definition);
  ~PerEntryHistogram(){;}

  void AccumulateCurrentEntry();

  void Write();
  
  TH1D*       mean;
  TH1D*       variance;
  int         n;
  std::string command;
  std::string selection;
  
private:
  PerEntryHistogram() = delete;
};

#endif
