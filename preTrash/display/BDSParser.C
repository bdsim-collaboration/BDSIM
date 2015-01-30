/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 12.06.2005
*/
#include "BDSParser.h"
#include "BDSPad.h"
#include "BDSCanvas.h"

using namespace std;

BDSParser::BDSParser()
{
	//*-*-*-*-*-*-*-*-*-*-*Line default constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
	//*-*                  ========================
}

void BDSParser::LoadFile(BDSPad* _pad, char* _path)
{
	gmad_parser(_path);
	list<struct Element>::iterator it;
	Double_t s_current=0.;
	Int_t nelement=1;
	TString temp; 
	((BDSCanvas*)gPad->GetCanvas())->EndPositionElement[nelement]=0.;
	_pad->Begin();
	for(it = beamline_list.begin();it!=beamline_list.end();it++)
	{
	  if((*it).l> 1e-9)
	    {
	      s_current += (*it).l;
	     
	      if((*it).type==_RCOL || (*it).type==_ECOL)
		{
		  ((BDSCanvas*)gPad->GetCanvas())->ElementXAper[nelement]=(*it).xsize;
		  ((BDSCanvas*)gPad->GetCanvas())->ElementYAper[nelement]=(*it).ysize;
		  
		}
	      else
		{
		  if((*it).aper!=0)
		    {
		      ((BDSCanvas*)gPad->GetCanvas())->ElementXAper[nelement]=(*it).aper;
		      ((BDSCanvas*)gPad->GetCanvas())->ElementYAper[nelement]=(*it).aper;
		    } 
		  
		  else
		    {
		      ((BDSCanvas*)gPad->GetCanvas())->ElementXAper[nelement]=options.beampipeRadius;
		      ((BDSCanvas*)gPad->GetCanvas())->ElementYAper[nelement]=options.beampipeRadius;
		    }
		}
		((BDSCanvas*)gPad->GetCanvas())->EndPositionElement[nelement]=s_current;
		temp=Form((*it).name);
		((BDSCanvas*)gPad->GetCanvas())->NameElement[nelement]=temp;	
		temp=Form(typestr((*it).type));
		((BDSCanvas*)gPad->GetCanvas())->TypeElement[nelement]=temp;	
		_pad->AddBDSBox(*it,s_current);
		cout << (*it).name << 
"\t\t"<<s_current-(*it).l<<"\t\t"<<s_current<<"\t\t"<<(*it).l<<endl;
		nelement++;
		}
	}
	((BDSCanvas*)gPad->GetCanvas())->NumberElement=nelement-1;
	cout << "total length "  << s_current<< endl;	
	_pad->End();
}

