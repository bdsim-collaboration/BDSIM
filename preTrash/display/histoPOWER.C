histoPOWER(char *gmadFile, char *rootFile)
{
	gStyle->SetOptTitle(0);
	BDSCanvas *b = new BDSCanvas(gmadFile);
	TFile *f = TFile::Open(rootFile);
	TH1F *histo;
	TAxis *axis;
	histo = (TH1F*)f->Get("ElossHisto;1"); 
	axis = histo->GetXaxis();
//	histo->GetXaxis()->SetLimits(BDSPad::xmin_axis,BDSPad::xmax_axis);
	axis->SetRange(BDSPad::xmin_axis,BDSPad::xmax_axis);
	//axis->SetRange(axis->FindBin(BDSPad::xmin_axis),axis->FindBin(BDSPad::xmax_axis));
	histo->Scale(1);
	histo->Draw();
	b->SetHisto(histo); 
	b->cd();
	return(0);
}
