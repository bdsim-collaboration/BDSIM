#include "BDSOutputFactory.hh"

#include "BDSOutputFormat.hh"
#include "BDSOutput.hh"
#include "BDSOutputNone.hh"
#include "BDSOutputROOT.hh"

#include "BDSDebug.hh"

BDSOutput* BDSOutputFactory::CreateOutput(BDSOutputFormat format)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format = " << format << G4endl;
#endif
  switch (format.underlying())
    {
    case BDSOutputFormat::none:
      {return new BDSOutputNone(); break;}
    case BDSOutputFormat::rootevent:
      {return new BDSOutputROOT(); break;}
    default:
      {return new BDSOutputNone(); break;} // absolute default - should not reach this
    }
}
