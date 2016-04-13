#include "BDSOutputFactory.hh"

#include "BDSOutputFormat.hh"
#include "BDSOutputBase.hh"
#include "BDSOutputASCII.hh"
#include "BDSOutputNone.hh"
#include "BDSOutputROOT.hh"
#include "BDSOutputROOTDetailed.hh"
#include "BDSOutputROOTEvent.hh"
#include "BDSOutputVector.hh"

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
    case BDSOutputFormat::combined:
      {
	BDSOutputVector* combinedOutput = new BDSOutputVector();
	combinedOutput->Add(new BDSOutputASCII());
	combinedOutput->Add(new BDSOutputROOT<float>());
	combinedOutput->Add(new BDSOutputROOTEvent());
	return combinedOutput;
	break;
      }
    case BDSOutputFormat::ascii:
      {return new BDSOutputASCII(); break;}
    case BDSOutputFormat::root:
      {return new BDSOutputROOT<float>(); break;}
    case BDSOutputFormat::rootdouble:
      {return new BDSOutputROOT<double>(); break;}
    case BDSOutputFormat::rootdetailed:
      {return new BDSOutputROOTDetailed<float>(); break;}
    case BDSOutputFormat::rootdetaileddouble:
      {return new BDSOutputROOTDetailed<double>(); break;}
    case BDSOutputFormat::rootevent:
      {return new BDSOutputROOTEvent(); break;}
    default:
      {return new BDSOutputNone(); break;} // absolute default - should not reach this
    }
}
