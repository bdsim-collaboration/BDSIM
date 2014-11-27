/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 06.12.2005
*/
#ifndef ROOT_TText
#include "TText.h"
#endif
#include "BDSConstant.h"
#include "BDSPopUp.h"
ClassImp(BDSPopUp) 

BDSPopUp::BDSPopUp(TString name, short type,Float_t len, Float_t s,
		   Float_t k0, Float_t k1, Float_t k2, Float_t k3,
		   Float_t angle, Float_t aper_x, Float_t aper_y)
: TCanvas("echo_canvas","Optics Specs.",960,10,330,200)
{
	TString temp;
	TString string_type;
	
	TText *t0 = new TText(0.02,0.9,"Opticals Elements specs.");
	temp= "Name: " + name;

	string_type=Form(typestr(type));
	temp = temp + " Type :" + string_type;
	TText *t1 = new TText(0.02,0.7,temp);
	temp=Form("@ %f m with a length of %f m",s,len);
	TText *t2 = new TText(0.02,0.6,temp);
	temp=Form("Aperture: X = %f m, Y = %f m",aper_x,aper_y);
	TText *t3 = new TText(0.02,0.5,temp);
	temp=Form("k0=%f, k1=%f",k0,k1);
	TText *t4 = new TText(0.02,0.3,temp);
	temp=Form("k2=%f, k3=%f",k2,k3);
	TText *t5 = new TText(0.02,0.2,temp);
	temp=Form("angle=%f",angle);
	TText *t6 = new TText(0.02,0.1,temp);
	
	t0->SetTextColor(2);
	t0->SetTextAlign(12);
	t0->SetTextSize(0.11);
	t1->SetTextAlign(12);
	t1->SetTextSize(0.09);
	t2->SetTextAlign(12);
	t2->SetTextSize(0.09);
	t3->SetTextAlign(12);
	t3->SetTextSize(0.09);
	t4->SetTextAlign(12);
	t4->SetTextSize(0.09);	
	t5->SetTextAlign(12);
	t5->SetTextSize(0.09);	
	t6->SetTextAlign(12);
	t6->SetTextSize(0.09);	
	
	t0->Draw();
	t1->Draw();
	t2->Draw();
	t3->Draw();
	t4->Draw();
	t5->Draw();
	t6->Draw();
}
