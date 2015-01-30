/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/
#ifndef __BDSPopUp__
#define __BDSPopUp__

#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
 
class BDSPopUp : public TCanvas
{
public:
	BDSPopUp(TString name, short type, Float_t len, Float_t s,
		 Float_t k1, Float_t k2, Float_t k3, Float_t k4, Float_t angle,
		 Float_t aper_x, Float_t aper_y);

public:
	ClassDef(BDSPopUp,1)
};
#endif
