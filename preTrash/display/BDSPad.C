/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 24.11.2005
*/
#include <iostream>
#include "BDSPad.h"
#include "BDSCanvas.h"
#include "BDSAxis.h"
#include "BDSConstant.h"
#include "BDSBox.h"
#include "BDSPopUp.h" 

ClassImp(BDSPad)

Double_t BDSPad::xmin_axis;
Double_t BDSPad::xmax_axis;

using namespace std;

Double_t Bdsaxis2Bdspad(Double_t axis_position, Double_t min, Double_t max)
{
	Double_t deltaxis = max-min;
	return min+deltaxis*(axis_position-xmin_axis_position)/(xmax_axis_position-xmin_axis_position);
}

BDSPad::~BDSPad()
{
	//	if(browser != NULL) delete browser;
	if(axis != NULL) delete axis;
}

BDSPad::BDSPad(BDSCanvas *_browser,const char* name, const char* title, Double_t xlow, Double_t ylow,
			   Double_t xup, Double_t yup, Color_t color)
:TPad(name, title, xlow, ylow, xup, yup, color), browser(_browser)
{
	axis = new BDSAxis(xmin_axis_position,y_axis_position,xmax_axis_position,y_axis_position,
						BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis(),bin_axis);
	
	axis->SetLabelSize(0.1);
	//axis->SetTitle("m");
	//axis->SetTitleSize(0.1);
	//axis->SetTitleOffset(0.3);
	this->Draw();
	this->cd();
	axis->Draw();
	this->AddExec("ex","BDSPad::BoxClicked()");
	//Clear all the default pop-up function...
	cl_line = this->GetAxis()->IsA();
	li_line = cl_line->GetMenuList();
	li_line->Clear();
	n_line = new TClassMenuItem(TClassMenuItem::kPopupUserFunction,cl_line,"BDSPad UnZoom","UnZoom",this,"");
	li_line->AddFirst(n_line);
	
}

void BDSPad::BoxClicked()
{
	int event = gPad->GetEvent();
	
	if (event != 11) return; //may be comment this line 
	TObject *select = gPad->GetSelected();
	if (!select) return;
	if (!select->InheritsFrom("BDSBox")){return;}
	BDSBox *element = (BDSBox*)select;
	
	TString name= element->GetBDSBoxName();
	short type= element->GetBDSType();
	Float_t len = element->GetBDSLen(); 
	Float_t s=element->GetBDSX2();
	Float_t k0=element->GetBDSK0();
	Float_t k1=element->GetBDSK1();
	Float_t k2=element->GetBDSK2();
	Float_t k3=element->GetBDSK3();
	Float_t angle=element->GetBDSAngle();
	Float_t aper_x=element->GetAperX();
	Float_t aper_y=element->GetAperY();
	BDSPopUp *echo = new BDSPopUp(name,type,len,s,k0,k1,k2,k3,angle,aper_x,aper_y);
}
void BDSPad::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
	py=py; //Only to avoid a warning from ROOT (unused parameter 'Int_t py')
	
	//This Function implante zoom and unzoom 
	// Both Pads are here correlated
	if (event == kButton1Down)
	{
		// Sets first value
		xmin = AbsPixeltoX(px);

		//Draw the virtual box (always this in ROOT when you ZOOM )
		px1old = px;
		py1old = this->YtoAbsPixel(gPad->GetUymin());
		px2old = px1old;
		py2old = this->YtoAbsPixel(gPad->GetUymax());
		gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
		gVirtualX->DrawBox(px1old, 169, px2old, 471, TVirtualX::kHollow);
		gVirtualX->SetLineColor(-1);
		
	}
	
	if(event == kButton1Motion)
	{
		gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
		gVirtualX->DrawBox(px1old, 169, px2old, 471, TVirtualX::kHollow);
		px2old = px;
		gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);	
		gVirtualX->DrawBox(px1old, 169, px2old, 471, TVirtualX::kHollow);
	}
	
	if(event == kButton1Up)
	{
		// Sets the second value
		xmax = AbsPixeltoX(px);
		if (xmin > xmax) 
		{
			Double_t temp = xmin;
			xmin   = xmax;
			xmax   = temp;
		}
		
		if(xmax - xmin > zoom_min)
		{
			// Avoid zooming outside the axis
			if(xmin > xmin_axis_position && xmax < xmax_axis_position && xmax > xmin_axis_position)
			{
				Double_t dmin = axis->GetWmin();
				Double_t dmax = axis->GetWmax();
				
				xmax = Bdsaxis2Bdspad(xmax, dmin, dmax);
				xmin = Bdsaxis2Bdspad(xmin, dmin, dmax);
				
				UpdateLine(xmin,xmax);
				//Modified(kTRUE);
				
				TH1F *ax = browser->GetHisto();
				if(ax)
				{
					ax->SetAxisRange(xmin,xmax);				
					browser->GetPlot()->Modified(kTRUE);
					cout << "histo " << ax->GetBinLowEdge(ax->GetMinimumBin()) << " " << ax->GetMaximumBin() << endl;
				}
				TH2F   *ax2 = (TH2F*)browser->GetPlot()->GetPrimitive("temp");
            if(ax2)
			   {
					cout << "ntuple " << endl;
					ax2->SetAxisRange(xmin,xmax);
				   browser->GetPlot()->Modified(kTRUE);
				}
			browser->GetPlot()->Update();	
			// change the min max value of the line for precision pb
			UpdateLine(browser->GetPlot()->GetUxmin(),browser->GetPlot()->GetUxmax());
			Modified(kTRUE);
			}
		}	
	}
	
	//Double Click = Unzoom
	if(event == 61 || event == 62 || event == 63 )
	{
		UpdateLine(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
		TH1F *ax = ((BDSCanvas*)gPad->GetCanvas())->GetHisto();
		if(ax)
		{
			ax->SetAxisRange(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
			gPad->Modified(kTRUE);
			browser->GetPlot()->Modified(kTRUE);
		}
		
		TH2F   *ax2 = (TH2F*)((BDSPad*)gPad->GetPrimitive("temp"));
	   if(ax2)
		{
		 ax2->SetAxisRange(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
       gPad->Modified(kTRUE);
       browser->GetPlot()->Modified(kTRUE);
		}
	}
}


void BDSPad::UnZoom()
{
	UpdateLine(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
	TH1F *ax = browser->GetHisto();
	if(ax)
	{
		ax->SetAxisRange(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
		browser->GetPlot()->Modified(kTRUE);
	}
	
	TH2F   *ax2 = (TH2F*)browser->GetPlot()->GetPrimitive("temp");
	if(ax2)
	{
		ax2->SetAxisRange(BDSPad::get_xmin_axis(),BDSPad::get_xmax_axis());
		browser->GetPlot()->Modified(kTRUE);
	}
}

void BDSPad::Begin()
{
	xmax_axis = 0;
}

void BDSPad::AddBDSBox(Element _item, Double_t s)
{
	Double_t y1=0.3;
	Double_t y2=0.7;
	Int_t fillcolor=17;
	Int_t linecolor=1;
	if(_item.type==_QUAD)
	{
	if((_item.k1)>0)
		{
			y1=0.2;
			y2=0.49;
		}
	else
		{
			y1=0.51;
			y2=0.8;
		}
	}
	
	if(_item.type==_ECOL || _item.type==_RCOL)
	{
	y1=0.2;
	y2=0.8;
	fillcolor=2;
	}
	
	BDSBox* box = new BDSBox(_item,y1,y2);
	box->SetBDSX2(s);
	box->SetFillColor(fillcolor);
	box->SetLineColor(linecolor);
	box->Update(this,axis->GetWmin(),axis->GetWmax());
	box->SetUniqueID(i);
	box_list.Add(box);
	i++;
}

void BDSPad::End()
{
	TIterator *iter = box_list.MakeIterator();
	if(iter == NULL) return ;
	
	BDSBox *box;
	while((box = (BDSBox*)iter->Next()) != NULL)
	{
		if(box->GetBDSX2() >  xmax_axis)  xmax_axis = box->GetBDSX2();
	}
	UpdateLine(xmin_axis,xmax_axis);
	cout << "Axis max: "<< xmax_axis << " m "<< endl;
}

void BDSPad::UpdateLine(Double_t xmin, Double_t xmax)
{
	axis->SetWmin(xmin);
	axis->SetWmax(xmax);
	TIterator *iter = box_list.MakeIterator();
	if(iter == NULL) return ;
	BDSBox *box;
	
	while((box = (BDSBox*)iter->Next()) != NULL)
	{
		box->Update(this,xmin,xmax);
	}
}
