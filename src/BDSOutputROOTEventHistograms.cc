#include "BDSOutputROOTEventHistograms.hh"

ClassImp(BDSOutputROOTEventHistograms)

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms()
{
  TH1D::AddDirectory(kFALSE);
  TH2D::AddDirectory(kFALSE);
};

BDSOutputROOTEventHistograms::~BDSOutputROOTEventHistograms()
{

};

#ifndef __ROOTBUILD__

G4int BDSOutputROOTEventHistograms::Create1DHistogram(G4String name, G4String title,
                                                      G4int nbins, G4double xmin, G4double xmax)
{
  this->histograms1D.push_back(new TH1D(name,title, nbins, xmin, xmax));
  return this->histograms1D.size();
}

G4int BDSOutputROOTEventHistograms::Create1DHistogram(G4String name, G4String title,
                                                      std::vector<double>& edges)
{
  Double_t* edgesD = new Double_t[edges.size()];
  for(int i=0;i<(int)edges.size();++i) {
    edgesD[i] = edges[i];
  }
  this->histograms1D.push_back(new TH1D(name,title,(Int_t)edges.size(),edgesD));
  return this->histograms1D.size();

  delete[] edgesD;
}

G4int BDSOutputROOTEventHistograms::Create2DHistogram(G4String name, G4String title,
                                                      G4int nxbins, G4double xmin, G4double xmax,
                                                      G4int nybins, G4double ymin, G4double ymax)
{
  this->histograms2D.push_back(new TH2D(name,title, nxbins, xmin, xmax, nybins, ymin, ymax));
  return this->histograms2D.size();
}

G4int BDSOutputROOTEventHistograms::Create2DHistogram(G4String name, G4String title,
                                                      std::vector<double>& xedges,
                                                      std::vector<double>& yedges)
{
  Double_t* xedgesD = new Double_t[xedges.size()];
  for(int i=0;i<(int)xedges.size();++i) {
    xedgesD[i] = xedges[i];
  }

  Double_t* yedgesD = new Double_t[yedges.size()];
  for(int i=0;i<(int)yedges.size();++i) {
    yedgesD[i] = xedges[i];
  }

  this->histograms2D.push_back(new TH2D(name.data(),title.data(), (Int_t)xedges.size(), xedgesD, (Int_t)yedges.size(), yedgesD));
  return this->histograms2D.size();

  delete[] xedgesD;
  delete[] yedgesD;
}

void BDSOutputROOTEventHistograms::Fill1DHistogram(G4int histoId, G4double value,
                                                   G4double weight)
{
  this->histograms1D[histoId]->Fill(value,weight);
}
void BDSOutputROOTEventHistograms::Fill2DHistogram(G4int histoId, G4double xValue, G4double yValue,
                                                   G4double weight)
{
  this->histograms2D[histoId]->Fill(xValue,yValue,weight);
}
#endif