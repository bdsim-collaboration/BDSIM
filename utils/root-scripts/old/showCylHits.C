{

Float_t pi = 3.14;

TFile f("output1.root");
//TBrowser b;
TTree *t = (TTree*)f->Get("samp1");

Int_t nentries = (Int_t)t->GetEntries();

TBranch *brX = t->GetBranch("x");
TBranch *brY = t->GetBranch("y");
TBranch *brZ = t->GetBranch("z");
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


Float_t Zmin = -5;
Float_t Zmax = 1;

Float_t X,Y,Z;
Int_t npart;

brX->SetAddress(&X);
brY->SetAddress(&Y);
brZ->SetAddress(&Z);
brNpart->SetAddress(&npart);

TH2F *histXY = new TH2F("histo","R #phi",100,Zmin,Zmax,100,-pi/2,pi/2);

//Int_t drawnPDGtype = 11; // electron
//histXY->SetTitle("Electrons"); 

//Int_t drawnPDGtype = -11; // electron
//histXY->SetTitle("Positrons"); 

Int_t drawnPDGtype = 22; // photons
histXY->SetTitle("Photons"); 


for( Int_t i=0; i<nentries;i++)
{
  brX->GetEntry(i);
  brY->GetEntry(i);
  brZ->GetEntry(i);
  brNpart->GetEntry(i);

  if(npart == drawnPDGtype) 
    histXY->Fill(Z,asin(Y/sqrt(X*X + Y*Y)));
}


histXY->SetMarkerColor(4);
histXY->SetMarkerStyle(6);
histXY->GetYaxis()->SetTitle("#phi , rad");
histXY->GetXaxis()->SetTitle("Z, m");

}
