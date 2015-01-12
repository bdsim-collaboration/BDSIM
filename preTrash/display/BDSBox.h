/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/
#ifndef __BDSBox__
#define __BDSBox__

#ifndef ROOT_TBox
#include "TBox.h"
#endif

#include "BDSParser.h"

#include "parser/gmad.h"

class BDSBox : public TBox
{
public:
	BDSBox(Element _item, Double_t _y1, Double_t _y2);	

	void Update(BDSPad* pad, Double_t xmin, Double_t xmax);
	Double_t GetBDSX1() {return s-item.l;}
   Double_t GetBDSX2() {return s;}
	TString  GetBDSBoxName(){return Form(item.name);}
	short GetBDSType(){return item.type;}
	Double_t GetBDSLen(){return item.l;}
	Float_t GetBDSAngle(){return item.angle;}

	Float_t GetBDSK0()
	{
	  if(item.type == _MULT){
	    if(item.knl.size()>0){
	      std::list<double>::iterator iter;
	      iter = item.knl.begin();
	      return (*iter);
            }
	  }
	else return item.k0;
	}

	Float_t GetBDSK1()
        {
          if(item.type == _MULT){
            if(item.knl.size()>1){
              std::list<double>::iterator iter;
              iter = item.knl.begin();
	      iter++;
              return (*iter);
            }
          }
        else return item.k1; 
        }

	Float_t GetBDSK2()
        {
          if(item.type == _MULT){
            if(item.knl.size()>2){
              std::list<double>::iterator iter;
              iter = item.knl.begin();
	      iter++; iter++;
              return (*iter);
            }
          }
        else return item.k2; 
        }

	Float_t GetBDSK3()
        {
          if(item.type == _MULT){
            if(item.knl.size()>3){
              std::list<double>::iterator iter;
              iter = item.knl.begin();
              iter++; iter++; iter++;
              return (*iter);
            }
          }
        else return item.k3;
        }

	Float_t GetBDSTilt(){return item.tilt;}
	Double_t SetBDSX2(Double_t S){s=S;}

	Float_t GetAperX()
	{
	  if(item.type==_RCOL || item.type==_ECOL)
	    return item.xsize;
	  else return item.aper;
	}
	Float_t GetAperY()
	{
	  if(item.type==_RCOL || item.type==_ECOL)
	    return item.ysize;
	  else return item.aper;
	}
	
private:
	Element item;
	Double_t s;

public:
	ClassDef(BDSBox,1)
};
#endif
