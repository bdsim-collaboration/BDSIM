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
#ifndef HISTOGRAMMEANFROMFILE_H
#define HISTOGRAMMEANFROMFILE_H

#include <vector>

class HistogramAccumulator;

class BDSOutputROOTEventHistograms;

class TDirectory;

class HistogramMeanFromFile
{
public:
  HistogramMeanFromFile(BDSOutputROOTEventHistograms* h);

  ~HistogramMeanFromFile();

  void Accumulate(BDSOutputROOTEventHistograms* hNew);
  void Terminate();

  void Write(TDirectory* dir = nullptr);

private:
  
  std::vector<HistogramAccumulator*> histograms1d;
  std::vector<HistogramAccumulator*> histograms2d;
  std::vector<HistogramAccumulator*> histograms3d;
};

#endif
