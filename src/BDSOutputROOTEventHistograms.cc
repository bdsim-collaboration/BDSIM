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
#include "BDSOutputROOTEventHistograms.hh"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include <boost/format.hpp>
#include <boost/histogram.hpp>

ClassImp(BDSOutputROOTEventHistograms)

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms()
{
  TH1D::AddDirectory(kFALSE);
  TH2D::AddDirectory(kFALSE);
  TH3D::AddDirectory(kFALSE);
}

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms(const BDSOutputROOTEventHistograms& rhs):
  TObject(rhs)
{
  Fill(&rhs);
}

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms(std::vector<TH1D*>& histograms1DIn,
							   std::vector<TH2D*>& histograms2DIn,
							   std::vector<TH3D*>& histograms3DIn,
							   std::vector<boost_histogram*>& histograms4DIn):
  histograms1D(histograms1DIn),
  histograms2D(histograms2DIn),
  histograms3D(histograms3DIn),
  histograms4D(histograms4DIn)
{;}

BDSOutputROOTEventHistograms::~BDSOutputROOTEventHistograms()
{;}

void BDSOutputROOTEventHistograms::FillSimple(const BDSOutputROOTEventHistograms* rhs)
{
  if (!rhs)
    {return;}

  histograms1D = rhs->histograms1D;
  histograms2D = rhs->histograms2D;
  histograms3D = rhs->histograms3D;
  histograms4D = rhs->histograms4D;
}

void BDSOutputROOTEventHistograms::Fill(const BDSOutputROOTEventHistograms* rhs)
{
  if (!rhs)
    {return;}

  // for each histogram, clone (ie copy) it into this object
  for (auto h : rhs->histograms1D)
    {histograms1D.push_back(static_cast<TH1D*>(h->Clone()));}
  for (auto h : rhs->histograms2D)
    {histograms2D.push_back(static_cast<TH2D*>(h->Clone()));}
  for (auto h : rhs->histograms3D)
    {histograms3D.push_back(static_cast<TH3D*>(h->Clone()));}
  for (auto h : rhs->histograms4D)
    {boost_histogram* h_clone = nullptr;
    boost_histogram h_tmp = *h;
    *h_clone = h_tmp;
    histograms4D.push_back(static_cast<boost_histogram*>(h_clone));}

}

int BDSOutputROOTEventHistograms::Create1DHistogramSTD(std::string name, std::string title,
						       int nbins, double xmin, double xmax)
{
  histograms1D.push_back(new TH1D(name.c_str(),title.c_str(), nbins, xmin, xmax));
  return (int)histograms1D.size() - 1;
}

#ifndef __ROOTBUILD__

G4int BDSOutputROOTEventHistograms::Create1DHistogram(G4String name, G4String title,
                                                      G4int nbins, G4double xmin, G4double xmax)
{
  histograms1D.push_back(new TH1D(name,title, nbins, xmin, xmax));
  return (G4int)histograms1D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create1DHistogram(G4String name, G4String title,
                                                      std::vector<double>& edges)
{

  Double_t* edgesD = new Double_t[edges.size()];
  for (int i=0;i<(int)edges.size();++i)
    {edgesD[i] = edges[i];}
    
  histograms1D.push_back(new TH1D(name,title,(Int_t)edges.size()-1,edgesD));
  delete[] edgesD;

  return (G4int)histograms1D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create2DHistogram(G4String name, G4String title,
                                                      G4int nxbins, G4double xmin, G4double xmax,
                                                      G4int nybins, G4double ymin, G4double ymax)
{
  histograms2D.push_back(new TH2D(name,title, nxbins, xmin, xmax, nybins, ymin, ymax));
  return (G4int)histograms2D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create2DHistogram(G4String name, G4String title,
                                                      std::vector<double>& xedges,
                                                      std::vector<double>& yedges)
{
  Double_t* xedgesD = new Double_t[xedges.size()];
  for (int i=0;i<(int)xedges.size();++i)
    {xedgesD[i] = xedges[i];}

  Double_t* yedgesD = new Double_t[yedges.size()];
  for (int i=0;i<(int)yedges.size();++i)
    {yedgesD[i] = yedges[i];}

  histograms2D.push_back(new TH2D(name.data(),title.data(), (Int_t)xedges.size()-1, xedgesD, (Int_t)yedges.size()-1, yedgesD));
  delete[] xedgesD;
  delete[] yedgesD;

  return (G4int)histograms2D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create3DHistogram(G4String name, G4String title,
						      G4int nxbins, G4double xmin, G4double xmax,
						      G4int nybins, G4double ymin, G4double ymax,
						      G4int nzbins, G4double zmin, G4double zmax)
{
  histograms3D.push_back(new TH3D(name, title,
				  nxbins, xmin, xmax,
				  nybins, ymin, ymax,
				  nzbins, zmin, zmax));
  return (G4int)histograms3D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create3DHistogram(G4String name, G4String title,
						      std::vector<double>& xedges,
						      std::vector<double>& yedges,
						      std::vector<double>& zedges)
{
  Double_t* xedgesD = new Double_t[xedges.size()];
  for (int i=0;i<(int)xedges.size();++i)
    {xedgesD[i] = xedges[i];}

  Double_t* yedgesD = new Double_t[yedges.size()];
  for (int i=0;i<(int)yedges.size();++i)
    {yedgesD[i] = yedges[i];}

  Double_t* zedgesD = new Double_t[zedges.size()];
  for (int i=0;i<(int)zedges.size();++i)
    {zedgesD[i] = zedges[i];}
  
  histograms3D.push_back(new TH3D(name.data(),title.data(),
				  (Int_t)xedges.size()-1, xedgesD,
				  (Int_t)yedges.size()-1, yedgesD,
				  (Int_t)zedges.size()-1, zedgesD));
  return (G4int)histograms3D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create4DHistogram(G4String name, G4String title,
                                unsigned int nxbins, G4double xmin, G4double xmax,
                                unsigned int nybins, G4double ymin, G4double ymax,
                                unsigned int nzbins, G4double zmin, G4double zmax,
                                unsigned int nebins, G4double emin, G4double emax)
{

    boost_histogram h_tmp = boost::histogram::make_histogram(boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
                            boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
                            boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
                            boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "Energy_log"});

    boost_histogram* h = new boost_histogram(h_tmp);
    histograms4D.push_back(h);

    return (G4int)histograms4D.size() - 1;
}

void BDSOutputROOTEventHistograms::Fill1DHistogram(G4int    histoId,
						   G4double value,
                                                   G4double weight)
{
  histograms1D[histoId]->Fill(value,weight);
}

void BDSOutputROOTEventHistograms::Fill2DHistogram(G4int    histoId,
						   G4double xValue,
						   G4double yValue,
                                                   G4double weight)
{
  histograms2D[histoId]->Fill(xValue,yValue,weight);
}

void BDSOutputROOTEventHistograms::Fill3DHistogram(G4int    histoId,
						   G4double xValue,
						   G4double yValue,
						   G4double zValue,
						   G4double weight)
{
  histograms3D[histoId]->Fill(xValue,yValue,zValue,weight);
}

void BDSOutputROOTEventHistograms::Fill4DHistogram(G4int histoId,
                            G4double xValue,
                            G4double yValue,
                            G4double zValue,
                            G4double eValue)
{
    histograms4D[histoId]->operator()(xValue, yValue, zValue, eValue);
}

void BDSOutputROOTEventHistograms::Set3DHistogramBinContent(G4int histoId,
							    G4int globalBinID,
							    G4double value)
{
  histograms3D[histoId]->SetBinContent(globalBinID, value);

}

void BDSOutputROOTEventHistograms::Set4DHistogramBinContent(G4int histoId,
                                G4int x,
                                G4int y,
                                G4int z,
                                G4int e,
                                G4double value)
{

    histograms4D[histoId]->at(x,y,z,e) = value;

}

void BDSOutputROOTEventHistograms::AccumulateHistogram3D(G4int histoId,
							 TH3D* otherHistogram)
{
  histograms3D[histoId]->Add(otherHistogram);
}

void BDSOutputROOTEventHistograms::AccumulateHistogram4D(G4int histoId,
                              boost_histogram*  otherHistogram)

{
    histograms4D[histoId]->operator+=(*otherHistogram);

}

#endif

void BDSOutputROOTEventHistograms::Flush()
{
  for (auto h : histograms1D)
    {h->Reset();}
  for (auto h : histograms2D)
    {h->Reset();}
  for (auto h : histograms3D)
    {h->Reset();}
  for (auto h : histograms4D)
    {h->reset();}
}
