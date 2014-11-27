/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/
#include "TROOT.h"
#include "TRint.h"
#include "TFile.h"
#include "TH1F.h"
#include "BDSCanvas.h"
#include "BDSPad.h"
int main(int argc, char **argv)
{
	TRint *theApp = new TRint("ROOT example", &argc, argv, NULL, 0);
	theApp->Run();
	
	return(0);
}
//--------------------------------------------------
