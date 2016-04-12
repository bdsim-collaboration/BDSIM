#include "BDSOutputROOTEventHistograms.hh"

ClassImp(BDSOutputROOTEventHistograms)

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms()
{

};

BDSOutputROOTEventHistograms::~BDSOutputROOTEventHistograms()
{

};

#ifndef __ROOTBUILD__
void BDSOutputROOTEventHistograms::Fill1DHistogram(G4int histoId, G4double value, G4double weight)
{
  this->histograms1D[histoId]->Fill(value,weight);
}
void BDSOutputROOTEventHistograms::Fill2DHistogram(G4int histoId, G4double xValue, G4double yValue, G4double weight)
{
  this->histograms2D[histoId]->Fill(xValue,yValue, weight);
}
#endif