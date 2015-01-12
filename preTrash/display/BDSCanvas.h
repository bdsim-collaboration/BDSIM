/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 12.06.2005
*/
#ifndef __BDSCanvas__
#define __BDSCanvas__

#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif

#ifndef ROOT_TH1F
#include "TH1F.h"
#endif

#ifndef ROOT_TH2F
#include "TH2F.h"
#endif

#ifndef ROOT_TPad
#include "TPad.h"
#endif

#ifndef ROOT_TSYSTEM
#include "TSystem.h"
#endif

#ifndef ROOT_TTEXT
#include "TText.h"
#endif
#define MAX_ELEMENTS 1000
class BDSPad;

class BDSCanvas : public TCanvas  
{
public:
	 BDSCanvas(char* opticfile);
	~BDSCanvas();
	
	void AxisClicked(); 
	static void AxisClickedCB(); 
	
	BDSPad *GetLine(){return line;}
	TPad *GetPlot(){return plot;}

	TH1F *GetHisto(){return h2;}
	TH1F *SetHisto(TH1F* temp) {h2=temp;}
	
	void UserInfo(char* _path);

	Double_t EndPositionElement[MAX_ELEMENTS];	
	Double_t* GetEndPositionElement() {return EndPositionElement;}
	
	TString TypeElement[MAX_ELEMENTS];
	TString* GetTypeElement() {return TypeElement;}
	TString NameElement[MAX_ELEMENTS];
	TString* GetNameElement() {return NameElement;}
	Int_t NumberElement;
	Int_t GetNumberElement() {return NumberElement;}	
	Double_t ElementXAper[MAX_ELEMENTS];
	Double_t* GetElementXAper() {return ElementXAper;}
	Double_t ElementYAper[MAX_ELEMENTS];
	Double_t* GetElementYAper() {return ElementYAper;}

private:		
	BDSPad *line;
	TPad *plot;
	TH1F *h2;
	
	Double_t xmin,xmax;
	Double_t xcut;

public:
	ClassDef(BDSCanvas,1)
	
};
#endif
