//
// Created by Eliott Ramoisiaux on 14/07/2020.
//

#ifndef BDSIM_HISTOGRAMDEF4D_HH
#define BDSIM_HISTOGRAMDEF4D_HH

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
    bool        logarithmicE;

ClassDef(HistogramDef4D, 1);
};


#endif //BDSIM_HISTOGRAMDEF4D_HH
