/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/
#ifndef __BDSPad__
#define __BDSPad__

#ifndef ROOT_TClass
#include "TClass.h"
#endif

#ifndef ROOT_TPad
#include "TPad.h"
#endif
#include "parser/gmad.h"

#ifndef ROOT_TClassMenuItem
#include "TClassMenuItem.h"
#endif

#ifndef ROOT_TList
#include "TList.h"
#endif

#define MAX 1000
class BDSCanvas;
class BDSAxis;
class BDSPad : public TPad
{
public:
	BDSPad(BDSCanvas *_browser,const char* name, const char* title, Double_t xlow, Double_t ylow,
		   Double_t xup, Double_t yup, Color_t color);
	~BDSPad();

	BDSAxis* GetAxis(){return axis;}
	
	static Double_t get_xmin_axis(){return xmin_axis;}
	static Double_t get_xmax_axis(){return xmax_axis;}
	
  void ExecuteEvent(Int_t event, Int_t px, Int_t py);
	void UnZoom();
	void AddBDSBox(Element _item, Double_t s);
	void Begin();
	void End();
	void UpdateLine(Double_t xmin, Double_t xmax);
	
	void BoxClicked();
	static Double_t xmin_axis;
	static Double_t xmax_axis;

private:
	BDSCanvas *browser;
	BDSAxis *axis;
	Double_t xmin, xmax;
	Int_t px1old, py1old, px2old, py2old;

	Int_t i;
	
	TList box_list;
	TList *li_line;
	TClassMenuItem *n_line;
	TClass *cl_line; 
	
public:
	ClassDef(BDSPad,1)
};
#endif 
