std::vector<TH1D*>& GetRun1DHistograms(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Run");
  BDSOutputROOTEventHistograms *h = 0;
  t->SetBranchAddress("Histos.",&h);
  t->GetEntry(0);
  return h->Get1DHistograms();
}

void DrawPhaseEllipse(TString fileName)
{
  TFile *f = new TFile(fileName.Data());
  TTree *t = (TTree*)f->Get("Options");
  BDSOutputROOTEventOptions *o = 0;
  t->SetBranchAddress("Options.",&o);
  t->GetEntry(0);


}