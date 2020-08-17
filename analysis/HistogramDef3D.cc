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
#include "HistogramDef3D.hh"

ClassImp(HistogramDef3D)

HistogramDef3D::HistogramDef3D():
  zNBins(0),
  zLow(0),
  zHigh(0),
  logarithmicZ(false),
  binEdgesZ(nullptr)
{;}

HistogramDef3D::HistogramDef3D(const std::string& treeNameIn,
			       const std::string& histNameIn,
			       int         xNBinsIn,
			       int         yNBinsIn,
			       int         zNBinsIn,
			       double      xLowIn,
			       double      xHighIn,
			       double      yLowIn,
			       double      yHighIn,
			       double      zLowIn,
			       double      zHighIn,
			       const std::string& plotIn,
			       const std::string& selectionIn,
			       bool        perEntryIn,
			       bool        logarithmicXIn,
			       bool        logarithmicYIn,
			       bool        logarithmicZIn):
  HistogramDef2D(treeNameIn, histNameIn, xNBinsIn, yNBinsIn,
		 xLowIn, xHighIn, yLowIn, yHighIn,
		 plotIn, selectionIn, perEntryIn,
		 logarithmicXIn, logarithmicYIn),
  zNBins(zNBinsIn),
  zLow(zLowIn),
  zHigh(zHighIn),
  logarithmicZ(logarithmicZIn),
  binEdgesZ(nullptr)
{
  nDimensions = 3;
}

HistogramDef3D::HistogramDef3D(const std::string&   treeNameIn,
			       const std::string&   histNameIn,
			       std::vector<double>* binEdgesXIn,
			       std::vector<double>* binEdgesYIn,
			       std::vector<double>* binEdgesZIn,
			       const std::string&   variableIn,
			       const std::string&   selectionIn,
			       bool                 perEntryIn):
  HistogramDef2D(treeNameIn, histNameIn, binEdgesXIn, binEdgesYIn, variableIn, selectionIn, perEntryIn),
  zNBins(1),
  zLow(0),
  zHigh(1),
  logarithmicZ(false),
  binEdgesZ(binEdgesZIn)
{
  nDimensions = 3;

  if (!binEdgesZIn)
    {throw std::invalid_argument("invalid bin edges vector.");}
  if (!(binEdgesZIn->size() > 1))
    {throw std::invalid_argument("too few bin edges.");}
  zNBins = (int)binEdgesZ->size() - 1;
  zLow   = (*binEdgesZ)[0];
  zHigh  = binEdgesZ->back();
}

HistogramDef3D::~HistogramDef3D()
{
  delete binEdgesZ;
}
