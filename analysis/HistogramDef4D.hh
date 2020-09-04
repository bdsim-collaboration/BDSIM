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
#ifndef BDSIM_HISTOGRAMDEF4D_H
#define BDSIM_HISTOGRAMDEF4D_H
#include "BinSpecification.hh"
#include "HistogramDef3D.hh"

#include <string>

#include "Rtypes.h" // for classdef

class HistogramDef4D : public HistogramDef3D
{
public:
    /// Public constructor only for compatibility with ROOT - not indended for use.
    HistogramDef4D();

    /// Use this constructor.
    HistogramDef4D(std::string treeNameIn,
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
    std::string selectionIn    = "1",
    bool        perEntryIn     = true,
    bool        logarithmicXIn = false,
    bool        logarithmicYIn = false,
    bool        logarithmicZIn = false,
    bool        logarithmicEIn = false);
    virtual ~HistogramDef4D(){;}

    int         eNBins;
    double      eLow;
    double      eHigh;
    std::string eScale;
    std::vector<double> eBinsEdges;

    bool        logarithmicE;

ClassDef(HistogramDef4D, 1);
};


#endif //BDSIM_HISTOGRAMDEF4D_HH
