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
#include "HistogramAccumulator.hh"

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include <cmath>
#include <string>


HistogramAccumulator::HistogramAccumulator(TH1*               baseHistogram,
					   const int&         nDimensionsIn,
					   const std::string  resultHistNameIn,
					   const std::string& resultHistTitleIn):
  nDimensions(nDimensionsIn),
  n(0),
  terminated(false),
  resultHistName(resultHistNameIn),
  resultHistTitle(resultHistTitleIn),
  result(nullptr)
{
  std::string meanName = resultHistName + "_Mean";
  std::string variName = resultHistName + "_Vari";
  switch (nDimensions)
    {
    case 1:
      {
	mean     = static_cast<TH1D*>(baseHistogram->Clone(meanName.c_str()));
	variance = static_cast<TH1D*>(baseHistogram->Clone(variName.c_str()));
	result   = static_cast<TH1D*>(baseHistogram->Clone(resultHistName.c_str()));
	break;
      }
    case 2:
      {
	mean     = static_cast<TH2D*>(baseHistogram->Clone(meanName.c_str()));
	variance = static_cast<TH2D*>(baseHistogram->Clone(variName.c_str()));
	result   = static_cast<TH2D*>(baseHistogram->Clone(resultHistName.c_str()));
	break;
      }
    case 3:
      {
	mean     = static_cast<TH3D*>(baseHistogram->Clone(meanName.c_str()));
	variance = static_cast<TH3D*>(baseHistogram->Clone(variName.c_str()));
	result   = static_cast<TH3D*>(baseHistogram->Clone(resultHistName.c_str()));
	break;
      }
    default:
      {break;}
    }
  result->Reset();
  result->SetTitle(resultHistTitle.c_str());
  mean->SetTitle(meanName.c_str());
  variance->SetTitle(variName.c_str());
}

HistogramAccumulator::~HistogramAccumulator()
{
  // deleting histograms removes them from currently open output file
  delete mean;
  delete variance;
  // leak result here as ROOT annoyingly requires this to be left
}

void HistogramAccumulator::Accumulate(TH1* newValue)
{
  // temporary variables
  double newMean = 0;
  double newVari = 0;

  n++;
  switch (nDimensions)
    {
    case 1:
      {
	TH1D* h1  = static_cast<TH1D*>(mean);
	TH1D* h1e = static_cast<TH1D*>(variance);
	TH1D* ht  = static_cast<TH1D*>(newValue);
	for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	  {
	    AccumulateSingleValue(h1->GetBinContent(j),
				  h1e->GetBinContent(j),
				  ht->GetBinContent(j),
				  newMean, newVari);
	    h1->SetBinContent(j, newMean);
	    h1e->SetBinContent(j, newVari);
	  }
	break;
      }
    case 2:
      {
	TH2D* h1  = static_cast<TH2D*>(mean);
	TH2D* h1e = static_cast<TH2D*>(variance);
	TH2D* ht  = static_cast<TH2D*>(newValue);
	for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= h1->GetNbinsY() + 1; ++k)
	      {
		AccumulateSingleValue(h1->GetBinContent(j,k),
				      h1e->GetBinContent(j,k),
				      ht->GetBinContent(j,k),
				      newMean, newVari);
		h1->SetBinContent(j, k, newMean);
		h1e->SetBinContent(j, k, newVari);
	      }
	  }
	break;
      }
    case 3:
      {
	TH3D* h1  = static_cast<TH3D*>(mean);
	TH3D* h1e = static_cast<TH3D*>(variance);
	TH3D* ht  = static_cast<TH3D*>(newValue);
	for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= h1->GetNbinsY() + 1; ++k)
	      {
		for (int l = 0; l <= h1->GetNbinsZ() + 1; ++l)
		  {
		    AccumulateSingleValue(h1->GetBinContent(j,k,l),
					  h1e->GetBinContent(j,k,l),
					  ht->GetBinContent(j,k,l),
					  newMean, newVari);
		    h1->SetBinContent(j, k, l, newMean);
		    h1e->SetBinContent(j, k, l, newVari);
		  }
	      }
	  }
	break;
      }
    default:
      {break;}
    }
}

TH1* HistogramAccumulator::Terminate()
{
  double factor = std::sqrt(1./(double)n);
  double mn     = 0; // temporary variable
  double std    = 0; // temporary variable
  double var    = 0; // temporary variable

  // lambda to avoid repeating calculation even if it's simple
  auto getSTD  = [](auto& varIn, auto& nIn) {return std::sqrt(varIn / ((double)nIn-1));};

  // note here we set the std to 0 if there's only one entry (ie n = 1) to avoid
  // division by zero and nans
  switch (nDimensions)
    {
    case 1:
      {
	for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
	  {
	    mn  = mean->GetBinContent(j);
	    var = variance->GetBinContent(j);
	    std = n > 1 ? getSTD(var, n) : 0;
	    result->SetBinContent(j, mn);
	    result->SetBinError(j,   factor*std);
	  }
	break;
      }
    case 2:
      {
	for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= result->GetNbinsY() + 1; ++k)
	      {
		mn  = mean->GetBinContent(j,k);
		std = n > 1 ? getSTD(var, n) : 0;
		result->SetBinContent(j, k, mn);
		result->SetBinError(j, k,   factor*std);
	      }
	  }
	break;
      }
    case 3:
      {
	for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
	  {
	    for (int k = 0; k <= result->GetNbinsY() + 1; ++k)
	      {
		for (int l = 0; l <= result->GetNbinsZ() + 1; ++l)
		  {
		    mn  = mean->GetBinContent(j,k,l);
		    std = n > 1 ? getSTD(var, n) : 0;
		    result->SetBinContent(j,k,l, mn);
		    result->SetBinError(j,k,l,   factor*std);
		  }
	      }
	  }
	break;
      }
    default:
      {break;}
    }
  result->SetEntries(n);
  return result;
}

void HistogramAccumulator::AccumulateSingleValue(const double&  oldMean,
						 const double&  oldVari,
						 const double&  x,
						 double&        newMean,
						 double&        newVari) const
{
  newMean = oldMean + ((x - oldMean) / n);
  newVari = oldVari + ((x - oldMean) * (x - newMean));
}
