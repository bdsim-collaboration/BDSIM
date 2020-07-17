//
// Created by Eliott Ramoisiaux on 14/07/2020.
//

#include "HistogramDef4D.hh"

ClassImp(HistogramDef4D)

HistogramDef4D::HistogramDef4D():
        eNBins(0),
        eLow(0),
        eHigh(0),
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
        eLow(eLowIn),
        eHigh(eHighIn),
        logarithmicE(logarithmicEIn)
{
    nDimensions = 4;
}
