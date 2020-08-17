/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "HistogramDef2D.hh"

ClassImp(HistogramDef2D)

HistogramDef2D::HistogramDef2D():
  yNBins(0),
  yLow(0),
  yHigh(0),
  logarithmicY(false),
  binEdgesY(nullptr)
{;}

HistogramDef2D::HistogramDef2D(const std::string& treeNameIn,
			       const std::string& histNameIn,
			       int                xNBinsIn,
			       int                yNBinsIn,
			       double             xLowIn,
			       double             xHighIn,
			       double             yLowIn,
			       double             yHighIn,
			       const std::string& variableIn,
			       const std::string& selectionIn,
			       bool               perEntryIn,
			       bool               logarithmicXIn,
			       bool               logarithmicYIn):
  HistogramDef1D(treeNameIn, histNameIn, xNBinsIn, xLowIn,
		 xHighIn, variableIn, selectionIn, perEntryIn, logarithmicXIn),
  yNBins(yNBinsIn),
  yLow(yLowIn),
  yHigh(yHighIn),
  logarithmicY(logarithmicYIn),
  binEdgesY(nullptr)
{
  nDimensions = 2;
}

HistogramDef2D::HistogramDef2D(const std::string&   treeNameIn,
			       const std::string&   histNameIn,
			       std::vector<double>* binEdgesXIn,
			       std::vector<double>* binEdgesYIn,
			       const std::string&   variableIn,
			       const std::string&   selectionIn,
			       bool                 perEntryIn):
  HistogramDef1D(treeNameIn, histNameIn, binEdgesXIn, variableIn, selectionIn, perEntryIn),
  yNBins(1),
  yLow(0),
  yHigh(1),
  logarithmicY(false),
  binEdgesY(binEdgesYIn)
{
  nDimensions = 2;

  if (!binEdgesYIn)
    {throw std::invalid_argument("invalid bin edges vector.");}
  if (!(binEdgesYIn->size() > 1))
    {throw std::invalid_argument("too few bin edges.");}
  yNBins = (int)binEdgesY->size() - 1;
  yLow   = (*binEdgesY)[0];
  yHigh  = binEdgesY->back();
}

HistogramDef2D::~HistogramDef2D()
{
  delete binEdgesY;
}
