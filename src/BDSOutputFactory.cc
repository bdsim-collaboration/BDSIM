#include "BDSDebug.hh"
#include "BDSOutputFactory.hh"
#include "BDSOutputType.hh"
#include "BDSOutput.hh"
#include "BDSOutputNone.hh"
#include "BDSOutputROOT.hh"


BDSOutput* BDSOutputFactory::CreateOutput(BDSOutputType format,
					  G4String      fileName,
					  G4int         fileNumberOffset)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format = " << format << G4endl;
#endif
  switch (format.underlying())
    {
    case BDSOutputType::none:
      {return new BDSOutputNone(); break;}
    case BDSOutputType::rootevent:
      {return new BDSOutputROOT(fileName, fileNumberOffset); break;}
    default:
      {return new BDSOutputNone(); break;} // absolute default - should not reach this
    }
}
