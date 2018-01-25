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
#include "HistogramDef.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "PerEntryHistogram.hh"

#include "TChain.h"
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
  nDimensions(definition->nDimensions),
  n(0),
  histName(definition->histName),
  selection(definition->selection),
  result(nullptr),
  terminated(false)
{
  std::string meanName = histName + "Mean";
  std::string variName = histName + "Variance";
  std::string tempName = histName + "MeanTemp";
  command = definition->variable + " >> " + tempName;
  
  switch (nDimensions)
    {
    case 1:
      {
	const HistogramDef1D* d = static_cast<const HistogramDef1D*>(definition);
	if (d->logarithmicX)
          {// note ROOT requires len(binEdges) = nBins + 1
	    std::vector<double> binEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    mean = new TH1D(meanName.c_str(), meanName.c_str(), d->xNBins, binEdges.data());
          }
	else
          {
	    mean = new TH1D(meanName.c_str(), meanName.c_str(),
			    d->xNBins, d->xLow, d->xHigh);
          }
	variance = static_cast<TH1D*>(mean->Clone(variName.c_str()));
	temp     = static_cast<TH1D*>(mean->Clone(tempName.c_str()));
	break;
      }
    case 2:
      {
	const HistogramDef2D* d = static_cast<const HistogramDef2D*>(definition);
	if (d->logarithmicX && d->logarithmicY)
          {
	    std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
	    mean = new TH2D(meanName.c_str(), meanName.c_str(),
			    d->xNBins, xBinEdges.data(),
			    d->yNBins, yBinEdges.data());
          }
	else if (d->logarithmicX)
          {
	    std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    mean = new TH2D(meanName.c_str(), meanName.c_str(),
			    d->xNBins, xBinEdges.data(),
			    d->yNBins, d->yLow, d->yHigh);
          }
	else if (d->logarithmicY)
          {
	    std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
	    mean = new TH2D(meanName.c_str(), meanName.c_str(),
			    d->xNBins, d->xLow, d->xHigh,
			    d->yNBins, yBinEdges.data());
          }
	else
          {
	    mean = new TH2D(meanName.c_str(), meanName.c_str(),
			    d->xNBins, d->xLow, d->xHigh,
			    d->yNBins, d->yLow, d->yHigh);
          }
	variance = static_cast<TH2D*>(mean->Clone(variName.c_str()));
	temp     = static_cast<TH2D*>(mean->Clone(tempName.c_str()));
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
	    mean = new TH3D(meanName.c_str(), meanName.c_str(),
			    d->xNBins, xBinEdges.data(),
			    d->yNBins, yBinEdges.data(),
			    d->zNBins, zBinEdges.data());
	  }
	else
	  {
	    mean = new TH3D(meanName.c_str(), meanName.c_str(),
			    d->xNBins, d->xLow, d->xHigh,
			    d->yNBins, d->yLow, d->yHigh,
			    d->zNBins, d->zLow, d->zHigh);
	  }
	variance = static_cast<TH3D*>(mean->Clone(variName.c_str()));
	temp     = static_cast<TH3D*>(mean->Clone(tempName.c_str()));
	break;
      }
	default:
	  {break;}
      }
}
      
void PerEntryHistogram::AccumulateCurrentEntry()
{
  if (terminated)
    {
      std::cerr << "PerEntryHistogram already terminated" << std::endl;
      exit(1);
    }
  
  // Fill the temporary histogram with 1 event - the current one
  // This is used as it doesn't matter if the variable is a vector
  // or singly valued - therefore we don't need to keep a map of
  // which variables to loop over and which not to.
  chain->Draw(command.c_str(), selection.c_str(), "goff", 1, 0);

  // temporary variables
  double newMean = 0;
  double newVari = 0;

  // update mean & variance
  switch (nDimensions)
    {
    case 1:
      {
	TH1D* h1  = static_cast<TH1D*>(mean);
	TH1D* h1e = static_cast<TH1D*>(variance);
	TH1D* ht  = static_cast<TH1D*>(temp);
	for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	  {
	    AccumulateSingleValue(h1->GetBinContent(j),
				  h1e->GetBinContent(j),
				  ht->GetBinContent(j),
				  n, newMean, newVari);
	    h1->SetBinContent(j, newMean);
	    h1e->SetBinContent(j, newVari);
	  }
      }
    case 2:
      {
	TH2D* h1  = static_cast<TH2D*>(mean);
	TH2D* h1e = static_cast<TH2D*>(variance);
	TH2D* ht  = static_cast<TH2D*>(temp);
	for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= h1->GetNbinsY() + 1; ++k)
	      {
		AccumulateSingleValue(h1->GetBinContent(j,k),
				      h1e->GetBinContent(j,k),
				      ht->GetBinContent(j,k),
				      n, newMean, newVari);
		h1->SetBinContent(j, k, newMean);
		h1e->SetBinContent(j, k, newVari);
	      }
	  }
      }
    case 3:
      {
	TH3D* h1  = static_cast<TH3D*>(mean);
	TH3D* h1e = static_cast<TH3D*>(variance);
	TH3D* ht  = static_cast<TH3D*>(temp);
	for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= h1->GetNbinsY() + 1; ++k)
	      {
		for (int l = 0; l <= h1->GetNbinsZ()+1; ++l)
		  {
		    AccumulateSingleValue(h1->GetBinContent(j,k,l),
					  h1e->GetBinContent(j,k,l),
					  ht->GetBinContent(j,k,l),
					  n, newMean, newVari);
		    h1->SetBinContent(j, k, l, newMean);
		    h1e->SetBinContent(j, k, l, newVari);
		  }
	      }
	  }
      }
    default:
      {return;}
    }
}

void PerEntryHistogram::AccumulateSingleValue(const double&  oldMean,
					      const double&  oldVari,
					      const double&  x,
					      unsigned long& nIn,
					      double&        newMean,
					      double&        newVari)
{
  nIn++; // incremented before used
  newMean = oldMean + (x - oldMean) / nIn;
  newVari = oldVari + ((x - oldMean) * (x - newMean));
}

void PerEntryHistogram::Terminate()
{
  double factor = std::sqrt(1./(double)n);
  double std    = 0; // temporary variable
  switch (nDimensions)
    {
    case 1:
      {
	// create a copy of the histogram with the same dimensions
	result    = static_cast<TH1D*>(mean->Clone(histName.c_str()));
	resultSTD = static_cast<TH1D*>(mean->Clone((histName + "_std").c_str()));
	for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
	  {
	    std = std::sqrt(variance->GetBinContent(j));
	    result->SetBinContent(j,    mean->GetBinContent(j));
	    result->SetBinError(j,      factor*std);
	    resultSTD->SetBinContent(j, mean->GetBinContent(j));
	    resultSTD->SetBinError(j,   std);
	  }
	break;
      }
    case 2:
      {
	result    = static_cast<TH2D*>(mean->Clone(histName.c_str()));
	resultSTD = static_cast<TH2D*>(mean->Clone((histName + "_std").c_str()));
	for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= result->GetNbinsY() + 1; ++k)
	      {
		std = std::sqrt(variance->GetBinContent(j,k));
		result->SetBinContent(j, k,    mean->GetBinContent(j,k));
		result->SetBinError(j, k,      factor*std);
		resultSTD->SetBinContent(j, j, mean->GetBinContent(j,k));
		resultSTD->SetBinError(j, k,   std);
	      }
	  }
	break;
      }
    case 3:
      {
	result    = static_cast<TH3D*>(mean->Clone(histName.c_str()));
	resultSTD = static_cast<TH3D*>(mean->Clone((histName + "_std").c_str()));
	for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= result->GetNbinsY() + 1; ++k)
	      {
		for (int l = 0; l <= result->GetNbinsZ()+1; ++l)
		  {
		    std = std::sqrt(variance->GetBinContent(j,k,l));
		    result->SetBinContent(j,k,l,    mean->GetBinContent(j,k,l));
		    result->SetBinError(j,k,l,      factor*std);
		    resultSTD->SetBinContent(j,k,l, mean->GetBinContent(j,k,l));
		    resultSTD->SetBinError(j,k,l,   std);
		  }
	      }
	  }
	result->SetEntries(n);
	resultSTD->SetEntries(n);
	break;
      }
    default:
      {break;}
    }
  
  // delete files to remove from output
  delete temp;
  delete mean;
  delete variance;
  terminated = true;
}

void PerEntryHistogram::Write()
{
  if (result)
    {result->Write();}
}
