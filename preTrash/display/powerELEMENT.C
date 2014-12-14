powerELEMENT(char *gmadFile, char *rootFile)
{
  gStyle->SetOptTitle(0);
  BDSCanvas *b = new BDSCanvas(gmadFile);
  TFile *f = TFile::Open(rootFile);
  TNtuple *Ntuple = (TNtuple*)f->Get("ElossNtuple");
  Int_t i=1,n=1;
  Double_t coeff = 3.72e9*312*50*1.602e-10/12860.0;
  char temp[256];
  TH1F *myhisto= new TH1F("myhisto","Histo from the Ntuple projection",
			  b->GetNumberElement(),b->GetEndPositionElement());
  sprintf(temp,"E*%lf",coeff);
  Ntuple->Project("myhisto","z",temp);
  for(Int_t i=1;i<=b->GetNumberElement();i++) 
    //	cout << "sample,range="<< b->GetNameElement()[i] << ";" << endl;
    cout << b->GetNameElement()[i] << " " << i << " " << b->GetEndPositionElement()[i] <<"  " << 
      myhisto->GetBinContent(i)<< endl;   
  
  for(Int_t i=1;i<b->GetNumberElement();i++)
    {
      myhisto->SetBinContent(i,myhisto->GetBinContent(i)/(b->GetEndPositionElement()[i]-b->GetEndPositionElement()[i-1]));
      cout << b->GetNameElement()[i]  << " " << b->GetTypeElement()[i] << endl;
      //++<< (b->GetEndPositionElement()[i]-b->GetEndPositionElement()[i-1]) << " " 
      //<< myhisto->GetBinContent(i)   << " "  
      //<<myhisto->GetBinContent(i)/(b->GetEndPositionElement()[i]-b->GetEndPositionElement()[i-1]) << endl;
    }
  
  //for(Int_t i=1;i<b->GetNumberElement();i++) 
  //	cout << b->GetNameElement()[i] << " " << i << " " << b->GetEndPositionElement()[i] <<"  " << 
  //	myhisto->GetBinContent(i)<< endl;	
  myhisto->GetXaxis()->SetLimits(BDSPad::xmin_axis,BDSPad::xmax_axis);
  myhisto->Draw();
  b->SetHisto(myhisto);
  b->cd();
}

