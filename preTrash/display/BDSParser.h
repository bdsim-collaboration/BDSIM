/* Display for BDSIM code     
Author: Olivier Dadoun, Laboratoire de l'Accelerateur Lineaire (LAL-IN2P3), Orsay (France)
<mailto:> dadoun@lal.in2p3.fr, 2005
Last modified 30.09.2005
*/
#ifndef __BDSParser__
#define __BDSParser__

#ifndef ROOT_TString
#include "TString.h"
#endif

#include "parser/gmad.h"

class BDSPad;

class BDSParser 
{
public:
	BDSParser();

	void LoadFile(BDSPad* _pad, char* _path);
	ClassDef(BDSParser,1)	
};
#endif
