{

TFile f("output1.root");
//TBrowser b;
TTree *t = (TTree*)f->Get("samp1");

Int_t nentries = (Int_t)t->GetEntries();

TBranch *brX = t->GetBranch("x");
TBranch *brY = t->GetBranch("y");
TBranch *brNpart = t->GetBranch("part");
TBranch *brE = t->GetBranch("E");

Float_t Emin = -0.005;
Float_t Emax = +0.005;

TH1F *histE = new TH1F("histE","Energy",100,Emin,Emax);

Float_t E;
brE->SetAddress(&E);

for( Int_t i=0; i<nentries;i++)
{
  brE->GetEntry(i);
  histE->Fill(E);
}


Float_t Xmin = -30e+5;
Float_t Xmax = 30e+5;
Float_t Ymin = -30e+5;
Float_t Ymax = 30e+5;

Float_t X,Y;
Int_t npart;

brX->SetAddress(&X);
brY->SetAddress(&Y);
brNpart->SetAddress(&npart);

TH2F *histXY = new TH2F("histo","XY",100,Xmin,Xmax,100,Ymin,Ymax);

Int_t drawnPDGtype = 11; // electron
histXY->SetTitle("Electrons"); 

//Int_t drawnPDGtype = -11; // positron
//histXY->SetTitle("Positrons"); 

//Int_t drawnPDGtype = 22; // photons
//histXY->SetTitle("Photons"); 


for( Int_t i=0; i<nentries;i++)
{
  brX->GetEntry(i);
  brY->GetEntry(i);
  brNpart->GetEntry(i);

  if(npart == drawnPDGtype) histXY->Fill(X,Y);
}


histXY->SetMarkerColor(4);
histXY->SetMarkerStyle(6);
histXY->GetXaxis()->SetTitle("X #mum");
histXY->GetYaxis()->SetTitle("Y #mum");

}
