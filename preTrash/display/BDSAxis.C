/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/

#include "BDSAxis.h"

ClassImp(BDSAxis)

BDSAxis::BDSAxis(Double_t xmin, Double_t ymin, Double_t xmax, Double_t ymax, Double_t wmin, Double_t wmax, Int_t ndiv = 50510)
:TGaxis(xmin, ymin,  xmax,  ymax, wmin,  wmax, ndiv)
{
//*-*-*-*-*-*-*-*-*-*-*Line default constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ========================
}
