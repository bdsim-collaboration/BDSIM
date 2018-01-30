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
#include "BinGeneration.hh"
#include "HistogramAccumulator.hh"
#include "HistogramDef.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "PerEntryHistogram.hh"

#include "TChain.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

PerEntryHistogram::PerEntryHistogram(const HistogramDef* definition,
				     TChain*             chainIn):
  chain(chainIn),
  selection(definition->selection)
{
  int nDimensions = definition->nDimensions;
  TH1* baseHist = nullptr;
  std::string histName = definition->histName;
  std::string baseName = histName + "_base";
  std::string tempName = histName + "Temp";
  command = definition->variable + " >> " + tempName;
  
  switch (nDimensions)
    {
    case 1:
      {
	const HistogramDef1D* d = static_cast<const HistogramDef1D*>(definition);
	if (d->logarithmicX)
          {// note ROOT requires len(binEdges) = nBins + 1
	    std::vector<double> binEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    baseHist = new TH1D(baseName.c_str(), baseName.c_str(), d->xNBins, binEdges.data());
          }
	else
          {
	    baseHist = new TH1D(baseName.c_str(), baseName.c_str(),
				d->xNBins, d->xLow, d->xHigh);
          }
	temp = static_cast<TH1D*>(baseHist->Clone(tempName.c_str()));
	break;
      }
    case 2:
      {
	const HistogramDef2D* d = static_cast<const HistogramDef2D*>(definition);
	if (d->logarithmicX && d->logarithmicY)
          {
	    std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
	    baseHist = new TH2D(baseName.c_str(), baseName.c_str(),
				d->xNBins, xBinEdges.data(),
				d->yNBins, yBinEdges.data());
          }
	else if (d->logarithmicX)
          {
	    std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    baseHist = new TH2D(baseName.c_str(), baseName.c_str(),
				d->xNBins, xBinEdges.data(),
				d->yNBins, d->yLow, d->yHigh);
          }
	else if (d->logarithmicY)
          {
	    std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
	    baseHist = new TH2D(baseName.c_str(), baseName.c_str(),
				d->xNBins, d->xLow, d->xHigh,
				d->yNBins, yBinEdges.data());
          }
	else
          {
	    baseHist = new TH2D(baseName.c_str(), baseName.c_str(),
				d->xNBins, d->xLow, d->xHigh,
				d->yNBins, d->yLow, d->yHigh);
          }
	temp = static_cast<TH2D*>(baseHist->Clone(tempName.c_str()));
	break;
      }
    case 3:
      {
	const HistogramDef3D* d = static_cast<const HistogramDef3D*>(definition);
	if (d->logarithmicX || d->logarithmicY || d->logarithmicZ)
	  {
	    std::vector<double> xBinEdges;
	    std::vector<double> yBinEdges;
	    std::vector<double> zBinEdges;
	    if (d->logarithmicX)
	      {xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);}
	    else
	      {xBinEdges = RBDS::LinSpace(d->xLow, d->xHigh, d->xNBins);}
	    if (d->logarithmicY)
	      {yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);}
	    else
	      {yBinEdges = RBDS::LinSpace(d->yLow, d->yHigh, d->yNBins);}
	    if (d->logarithmicZ)
	      {zBinEdges = RBDS::LogSpace(d->zLow, d->zHigh, d->zNBins);}
	    else
	      {zBinEdges = RBDS::LinSpace(d->zLow, d->zHigh, d->zNBins);}
	    baseHist = new TH3D(baseName.c_str(), baseName.c_str(),
				d->xNBins, xBinEdges.data(),
				d->yNBins, yBinEdges.data(),
				d->zNBins, zBinEdges.data());
	  }
	else
	  {
	    baseHist = new TH3D(baseName.c_str(), baseName.c_str(),
				d->xNBins, d->xLow, d->xHigh,
				d->yNBins, d->yLow, d->yHigh,
				d->zNBins, d->zLow, d->zHigh);
	  }
	temp = static_cast<TH3D*>(baseHist->Clone(tempName.c_str()));
	break;
      }
	default:
	  {break;}
      }
  temp->SetTitle(tempName.c_str());

  accumulator = new HistogramAccumulator(baseHist, nDimensions, histName, histName);
}
      
void PerEntryHistogram::AccumulateCurrentEntry(const int& entryNumber)
{  
  // Fill the temporary histogram with 1 event - the current one
  // This is used as it doesn't matter if the variable is a vector
  // or singly valued - therefore we don't need to keep a map of
  // which variables to loop over and which not to.
  temp->Reset();
  chain->Draw(command.c_str(), selection.c_str(), "goff", 1, entryNumber);
  accumulator->Accumulate(temp);
}

void PerEntryHistogram::Terminate()
{
  result = accumulator->Terminate();
}

void PerEntryHistogram::Write(TDirectory* dir)
{
  if (result)
    {
      if (dir)
	{dir->Add(result);}
      result->Write();
    }
}
