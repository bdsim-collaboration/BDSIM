#include "HistogramMerge.hh"
#include "Config.hh"
#include "rebdsim.hh"

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

void HistogramMerge::Add(BDSOutputROOTEventHistograms *hIn)
{
  // loop over 1d histograms
  auto h1i = hIn->Get1DHistograms();
  for(size_t i=0;i<h1i.size();++i)
  {
    auto h1  = this->histograms1D[i];
    auto h1e = this->histograms1DError[i];
    h1e->SetName((std::string(h1->GetName())+"Error").c_str());
    auto h2  = hIn->Get1DHistograms()[i];
    // loop over bins
    for(int j=0;j<=h1->GetNbinsX()+1;++j)
    {
      h1->SetBinContent(j,h1->GetBinContent(j)+h2->GetBinContent(j));
      h1e->SetBinContent(j,h1e->GetBinContent(j)+pow(h2->GetBinContent(j),2));
    }
    this->histograms1DN[i]    = this->histograms1DN[i]+1;
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
  if(Config::Instance()->Debug())
  {
    std::cout << "terminate " << std::endl;
  }
  // loop over 1d histograms
  for(unsigned int i=0;i< histograms1D.size();++i)
  {
    auto h1  = histograms1D[i];
    auto h1e = histograms1DError[i];

    for(int j=0;j<=h1->GetNbinsX()+1;++j)
    {
      double mean = h1->GetBinContent(j)/histograms1DN[i];
      double std  = sqrt(h1e->GetBinContent(j)/histograms1DN[i]-pow(mean,2))/sqrt(histograms1DN[i]);
      h1->SetBinContent(j,mean);
      h1->SetBinError(j,std);
    }
  }
}

void HistogramMerge::Write(TFile* /*outputFile*/)
{
  // ROOT just writes object to the open file
  // is there a way to do this explictly?  kDirectory?
  // TBC commented out outputFile to prevent compiler warning
  for(auto h : histograms1D)
  {h->Write();}
  for(auto h : histograms2D)
  {h->Write();}
}
