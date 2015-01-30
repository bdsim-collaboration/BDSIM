/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/
#include "BDSBox.h"
#include "BDSConstant.h"
#include "BDSPad.h"

ClassImp(BDSBox)


Double_t Bdsbox2Bdsaxis(Double_t real_position, Double_t min, Double_t max)
{
	Double_t deltaxis_position=xmax_axis_position-xmin_axis_position;
	Double_t deltaxis = max-min;
	if( real_position > max )  real_position = max;
	if( real_position < min )  real_position = min;
	return (real_position-min)*deltaxis_position/deltaxis+xmin_axis_position;
}

BDSBox::BDSBox(Element _item, Double_t _y1, Double_t _y2)
: item(_item)
{
	SetY1(_y1);
	SetY2(_y2);	
}	

void BDSBox::Update(BDSPad* pad, Double_t xmin, Double_t xmax)
{
	Double_t xx1 = Bdsbox2Bdsaxis(GetBDSX1(),xmin,xmax);
	Double_t xx2 = Bdsbox2Bdsaxis(GetBDSX2(),xmin,xmax);	
	SetX1(xx1);
	SetX2(xx2);		
	
	if(pad)
	{
		if(xx2 < BDSPad::get_xmin_axis() || xx1 > BDSPad::get_xmax_axis())
		{
			pad->GetListOfPrimitives()->Remove(this);
		}
		else
		{
			
			if(item.type==_QUAD || item.type==_SBEND || item.type==_SEXTUPOLE || item.type==_ECOL || item.type==_RCOL || item.type==_OCTUPOLE || item.type==_MULT)
			{
				pad->GetListOfPrimitives()->Add(this);
			}
		
		}
	}
}
