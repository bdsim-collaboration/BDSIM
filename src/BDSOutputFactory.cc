#include "BDSOutputFactory.hh"

#include "BDSOutputFormat.hh"
#include "BDSOutputBase.hh"
#include "BDSOutputNone.hh"
#include "BDSOutputROOTEvent.hh"

#include "BDSDebug.hh"

BDSOutputBase* BDSOutputFactory::CreateOutput(BDSOutputFormat format)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format = " << format << G4endl;
#endif
  switch (format.underlying())
    {
    case BDSOutputFormat::none:
      {return new BDSOutputNone(); break;}
    case BDSOutputFormat::rootevent:
      {return new BDSOutputROOTEvent(); break;}
    default:
      {return new BDSOutputNone(); break;} // absolute default - should not reach this
    }
}
