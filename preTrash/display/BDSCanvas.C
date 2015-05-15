/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 23.11.2005
*/
#include <iostream>

#ifndef ROOT_TGaxis
#include "TGaxis.h"
#endif

#ifndef ROOT_TClassMenuItem
#include "TClassMenuItem.h"
#endif

#include "BDSConstant.h"
#include "BDSCanvas.h"
#include "BDSPad.h"
#include "BDSParser.h"
ClassImp(BDSCanvas) 

Int_t px1old,py1old,px2old,py2old;	
BDSCanvas::BDSCanvas(char* opticfile):TCanvas("BDSIMBrowser","BDSIM ROOT Framework",10,10,800,600)
{		
	this->cd();	
	TPad *info = new TPad("info", "this info pad", 0., 0.,1.,1,0);
	info->Draw();
	info->cd();
	UserInfo(opticfile);
	
	this->cd();
	line = new BDSPad(this,"line","this a line", 0.,0.75,1.,1,0);
   this->cd();	
	plot = new TPad("plot", "this a plot", 0., 0.05,1.,0.75,0);
	plot->SetLeftMargin(xmin_axis_position);
	plot->SetRightMargin(1.-xmax_axis_position);
	plot->Draw();

	BDSParser parser;
	parser.LoadFile(line,opticfile);
	plot->cd();
	plot->AddExec("ex","BDSCanvas::AxisClickedCB()");
}
BDSCanvas::~BDSCanvas()
{
	if(line != NULL) delete line;	
	if(plot != NULL) delete plot;	
	if(h2 != NULL) delete h2;
}

void BDSCanvas::AxisClickedCB()
{
	((BDSCanvas*)gPad->GetCanvas())->AxisClicked();
}

void BDSCanvas::AxisClicked()
{
	int event = plot->GetEvent();
	//else Box don't disappear
	TGaxis *plot_axis;
	plot_axis = (TGaxis*)line->GetAxis();
	TObject *select = plot->GetSelected();
	
	if (!select || !select->InheritsFrom("TAxis")) return;
	
	if (event == kButton1Down)
	{
	//Draw the virtual box (always this in ROOT when you ZOOM )
		px1old = GetEventX();
		py1old = 116;
		px2old = px1old;
		py2old = 16;
		gVirtualX->DrawBox(px1old,py1old,px2old,py2old,TVirtualX::kHollow);
		gVirtualX->SetLineColor(-1);
	}
	
	if(event == kButton1Motion)
	{
	gVirtualX->DrawBox(px1old,py1old,px2old,py2old,TVirtualX::kHollow);
	px2old = GetEventX();
	gVirtualX->DrawBox(px1old,py1old,px2old,py2old,TVirtualX::kHollow);
	}
										
	//Need this: update plot before line 
	if(event == kButton1Up)gPad->Update();
	xmin=gPad->GetUxmin();
	xmax=gPad->GetUxmax();

	plot_axis->SetWmax(xmax);
	plot_axis->SetWmin(xmin);
	line->UpdateLine(xmin,xmax);				
	line->Modified(kTRUE);
	
	//Double Click = Unzoom
	if(event == 61 || event == 62 || event == 63 )
	{
		TH1F *ax = ((BDSCanvas*)gPad->GetCanvas())->GetHisto();
		if(ax)
		{
			ax->SetAxisRange(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
			gPad->Modified(kTRUE);
		}
		
		TH2F   *ax2 = (TH2F*)((BDSPad*)gPad->GetPrimitive("temp"));
		if(ax2)
		{
			ax2->SetAxisRange(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
			gPad->Modified(kTRUE);
		}
		
		BDSCanvas *bro =(BDSCanvas*)gPad->GetCanvas();
		bro->GetLine()->UpdateLine(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
		bro->GetLine()->Modified(kTRUE);
	}
}


void BDSCanvas::UserInfo(char* _path){
	TString date;
	TString user;
	TString temp;
	FILE *fp = gSystem->OpenPipe("date", "r");
	date.Gets(fp);
	gSystem->ClosePipe(fp);
	fp = gSystem->OpenPipe("echo $USER","r");
	user.Gets(fp);
	temp = user + ", " + date;
    TText *infodate = new TText(0.015,0.011,temp);
    infodate->SetTextSize(0.02);
    infodate->Draw();

	temp = "BDSIM input file : " ;
	temp= temp + Form(_path);
	TText *infooptics=new TText(0.7,0.011,temp);
	infooptics->SetTextSize(0.02);
	infooptics->Draw();
}
