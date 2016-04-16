#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventHistograms.hh"

std::vector<TH1D*>& GetRun1DHistograms(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Run");
  BDSOutputROOTEventHistograms *h = 0;
  t->SetBranchAddress("Histos.",&h);
  t->GetEntry(0);
  return h->Get1DHistograms();
}

BDSOutputROOTEventOptions* GetOptions(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Options");
  BDSOutputROOTEventOptions *h = 0;
  t->SetBranchAddress("Options.",&h);
  t->GetEntry(0);
  return h;
}
