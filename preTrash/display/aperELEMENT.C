aperELEMENT(char *gmadFile, int x=1, int y=1)
{
  gStyle->SetOptTitle(0);
  BDSCanvas *b = new BDSCanvas(gmadFile);
  
  TH1F *aper_x = new TH1F("Apertures X","Apertures X",b->GetNumberElement(),b->GetEndPositionElement());
  TH1F *aper_y = new TH1F("Apertures Y","Apertures Y",b->GetNumberElement(),b->GetEndPositionElement());
  
  for(Int_t i=1;i<b->GetNumberElement();i++)
    {
      aper_x->SetBinContent(i,b->GetElementXAper()[i]);
      aper_y->SetBinContent(i,b->GetElementYAper()[i]);
    }
  aper_x->SetStats(0);
  aper_x->SetLineColor(2);
  aper_x->SetLineWidth(1);
  aper_y->SetLineColor(4);
  aper_y->SetLineWidth(1);
  if(x) aper_x->Draw();
  aper_x->GetXaxis()->SetTitle("Distance from IP [m]");
  aper_x->GetYaxis()->SetTitle("Apertures [m]");
  if(x && y )aper_y->Draw("same");
  if(!x && y) aper_y->Draw();
  if (x && y)
    {
      leg = new TLegend(0.9,0.87,0.99,0.99);
      char title[20];
      leg->AddEntry(aper_x,"Horiz.","l");
      leg->AddEntry(aper_y,"Vert.","l");
      leg->Draw();
    }
  b->cd();
}

