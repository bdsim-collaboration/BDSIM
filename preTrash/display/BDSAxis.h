/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/
#ifndef __BDSAxis__
#define __BDSAxis__

#ifndef ROOT_TGaxis
#include "TGaxis.h"
#endif


class BDSAxis : public TGaxis
{
public:
	BDSAxis(Double_t xmin, Double_t ymin, Double_t xmax, Double_t ymax, Double_t wmin, Double_t wmax, Int_t ndiv);
	
	ClassDef(BDSAxis,1)
};
#endif
