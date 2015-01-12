TH1F* joinhistograms_SG() 
{
  TChain ch("melody");
  ch.Add("*_*.root");
  TObjArray* files = ch.GetListOfFiles();
 
  TIter next(files);
  TChainElement *chEl=0;

  chEl=(TChainElement*)next();
  TFile  m_f0(chEl->GetTitle());
  cout << "opened file0: "<< (chEl->GetTitle()) << endl;
  TH1F* dummyHist0 = (TH1F*)m_f0.Get("ElossHisto");
//  dummyHist0.Sumw2();

  TH1F* outHist = (TH1F*)dummyHist0->Clone();
  outHist->SetName("outHist");
//  outHist.Sumw2();
  outHist->SetDirectory(0); // "detach" the histogram from the file
  m_f0.Close();

  int nfiles=1;

  while (( chEl=(TChainElement*)next() )) {
    TFile* m_f = new TFile(chEl->GetTitle());
    cout << "opened file: "<< (chEl->GetTitle()) << endl;

    TH1F* dummyHist = (TH1F*)m_f->Get("ElossHisto");    
//    dummyHist.Sumw2();
    outHist->Add(dummyHist);
    m_f->Close();
    nfiles++;
  }


//  double normalisation = 1.0/((double)nfiles*4000.0);
//  outHist->Scale(normalisation);
 
  TCanvas *c_eloss = new TCanvas("eloss", "Energy loss", 0, 0, 800, 600);
  c_eloss->SetLogy();
  outHist->Draw();

  return outHist;
}  
