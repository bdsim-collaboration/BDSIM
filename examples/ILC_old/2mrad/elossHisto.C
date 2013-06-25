{ 
  //TFile inpf1("eloss_tmp1.root");
  //TFile inpf2("eloss_tmp3_halo_nosr.root");
  //TFile inpf3("eloss_tmp4_nosr.root"); // no sr - beam
  //TFile inpf4("eloss_tmp2_halo.root");
  TFile inpf("output_0.root"); // no sr - beam
  

  TTree *tr = ElossNtuple;
  
  Int_t nbins = 200;

  TH1F *h = new TH1F("heloss","Energy loss [GeV/e-]",nbins,0,50);

  TH1F *hint = new TH1F("helossint","Integrated energy loss [GeV/e-]",nbins,0,50);

  Float_t E, z;

  tr->SetBranchAddress("z",&z);
  tr->SetBranchAddress("E",&E);

  Int_t nentries = tr->GetEntries();

  Float_t nfact = 1./500.;  // count Gev/e-

  for(Int_t i=0;i<nentries;i++)
    {
      tr->GetEntry(i);
      h->Fill(z,E*nfact);
    }
  
  Float_t x;
  
  for(Int_t i=0;i<nbins-1;i++)
    {
      x = 0;

      for(Int_t j=0;j<i+1;j++)
	{
	  x = x + h->GetBinContent(j);
	}

      hint->SetBinContent(i,x);

      //cout<<"i="<<i<<" x="<<x<<endl;

    }
  


}
