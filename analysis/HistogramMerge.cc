#include "HistogramMerge.hh"

ClassImp(HistogramMerge);


HistogramMerge::HistogramMerge()
{

}

HistogramMerge::HistogramMerge(BDSOutputROOTEventHistograms *h)
{
  auto h1i = h->Get1DHistograms();
  for(auto h : h1i)
  {
    auto ch  = (TH1D*)h->Clone();
    auto chE = (TH1D*)h->Clone();
    ch->Reset();
    chE->Reset();
    this->histograms1D.push_back(ch);
    this->histograms1DN.push_back(0);
    this->histograms1DError.push_back(chE);
  }

  auto h2i = h->Get2DHistograms();
  for(auto h : h2i)
  {
    auto ch  = (TH2D*)h->Clone();
    auto chE = (TH2D*)h->Clone();
    ch->Reset();
    chE->Reset();
    this->histograms2D.push_back(ch);
    this->histograms2DN.push_back(0);
    this->histograms2DError.push_back(chE);
  }
}

HistogramMerge::~HistogramMerge()
{

}

void HistogramMerge::Add(BDSOutputROOTEventHistograms *h)
{
  // loop over 1d histograms
  auto h1i = h->Get1DHistograms();
  for(size_t i=0;i<h1i.size();++i)
  {
    this->histograms1D[i]->Add(h1i[i]);
  }

#if 0
  // loop over 2d histograms
  auto h2i = h->Get2DHistograms();
  for(size_t i=0;i<h2i.size();++i)
  {
    this->histograms2D[i]->Add(h2i[i]);
  }
#endif
}

void HistogramMerge::Terminate()
{

}

void HistogramMerge::Write(TFile *outputFile)
{
  for(auto h : histograms1D)
  {
    h->Write();
  }
  for(auto h : histograms2D)
  {
    h->Write();
  }
}