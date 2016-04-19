#include "Utility.H"

#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1D.h"

#include "Event.hh"

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

BDSOutputROOTEventModel* GetModel(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Model");
  BDSOutputROOTEventModel *h = 0;
  t->SetBranchAddress("Model.",&h);
  t->GetEntry(0);
  return h;
}

void GetEvent(TString fileName, TChain *c, Event *e)
{
  c->Add(fileName.Data());
  std::vector<std::string> samplerNames;
  e->SetBranchAddress(c,samplerNames);
  return;
}