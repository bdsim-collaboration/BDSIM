chainTRACKING(char *gmadFile, char *rootFile)
{
	gStyle->SetOptTitle(0);
	BDSCanvas *b = new BDSCanvas(gmadFile);
	TChain *PChain;
	Char_t ChainName[256];
	Int_t jstar=1;
	Int_t jend=254;
	Float_t xmin=1000.,xmax=-1000.;
	Float_t ymin=1000.,ymax=-1000.;
	Float_t x,xp,y,yp,z,e;		
	Char_t temp[256];
	TNtuple *ntuple = new TNtuple("ntuple","nutple","e:xp:yp:z:x:y");
	for (Int_t j=jstar; j <= jend ; j++)
   {
    	sprintf(ChainName,"samp%d",j);
    	PChain=new TChain(ChainName,"chain");
    	PChain->Add(rootFile);
    	PChain->SetBranchAddress("x", &x);
    	PChain->SetBranchAddress("xp", &xp);
   	PChain->SetBranchAddress("y", &y);
    	PChain->SetBranchAddress("yp", &yp);
    	PChain->SetBranchAddress("z", &z);
    	PChain->SetBranchAddress("E", &e);
    	for (Int_t i=0; i<PChain->GetEntries(); i++)
   	{
			PChain->GetEntry(i);
			ntuple->Fill(e,xp,yp,z,x,y);
			ymin=TMath::Min(y,ymin);
			ymax=TMath::Max(y,ymax);
			
		}	
	}
	sprintf(temp,"x:z>>+temp(100,0,%f,50,%f,%f)",line->xmax_axis,1.5*ymin,1.5*ymax);
	ntuple->Draw(temp,"e==500","L");
	//ntuple->Draw(temp,"e==150","Lsame");
	//ntuple->Draw(temp,"e==200","Lsame");
	//ntuple->Draw(temp,"e==250","Lsame");
 	b->SetHisto(0);
//	TH2F *histo = new TH2F("histo","using histo",jend,line->xmin_axis,line->xmax_axis,jstar,1.5*xmin,1.5*xmax);
//	ntuple->Project("histo","x:z","e==200");
//	histo->Draw("e");
//
//	TH2F   *ax = b->GetPlot();
//b->SetHisto2(ax);
 	return(0);
}
