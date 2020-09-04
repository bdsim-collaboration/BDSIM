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
#include "HistogramDef4D.hh"

ClassImp(HistogramDef4D)

HistogramDef4D::HistogramDef4D():
        eNBins(0),
        eLow(0),
        eHigh(0),
        eScale("log"),
        eBinsEdges({}),
        logarithmicE(false)
{;}

HistogramDef4D::HistogramDef4D(std::string treeNameIn,
                               std::string histNameIn,
                               int         xNBinsIn,
                               int         yNBinsIn,
                               int         zNBinsIn,
                               int         eNBinsIn,
                               double      xLowIn,
                               double      xHighIn,
                               double      yLowIn,
                               double      yHighIn,
                               double      zLowIn,
                               double      zHighIn,
                               double      eLowIn,
                               double      eHighIn,
                               std::string eScaleIn,
                               std::vector<double> eBinsEdgesIn,
                               std::string plotIn,
                               std::string selectionIn,
                               bool        perEntryIn,
                               bool        logarithmicXIn,
                               bool        logarithmicYIn,
                               bool        logarithmicZIn,
                               bool        logarithmicEIn):
        HistogramDef3D(treeNameIn, histNameIn, xNBinsIn, yNBinsIn, zNBinsIn,
                       xLowIn, xHighIn, yLowIn, yHighIn, zLowIn, zHighIn,
                       plotIn, selectionIn, perEntryIn,
                       logarithmicXIn, logarithmicYIn, logarithmicZIn),
        eNBins(eNBinsIn),
        eScale(eScaleIn),
        eBinsEdges(eBinsEdgesIn),
        eLow(eLowIn),
        eHigh(eHighIn),
        logarithmicE(logarithmicEIn)
{
    nDimensions = 4;
}
