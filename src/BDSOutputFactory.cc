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
  if (format == BDSOutputFormat::none)
    {return new BDSOutputNone();}
  
  if (format == BDSOutputFormat::combined)
    {
      BDSOutputVector* combinedOutput = new BDSOutputVector();
      combinedOutput->Add(new BDSOutputASCII());
      combinedOutput->Add(new BDSOutputROOT());
      return combinedOutput;
    }
  
  else if (format == BDSOutputFormat::ascii)
    {return new BDSOutputASCII();}
  
  else if (format == BDSOutputFormat::root)
    {
    return new BDSOutputROOT();
    }
  else if (format == BDSOutputFormat::rootdetailed) 
    {
      return new BDSOutputROOTDetailed();
    }
  else if (format == BDSOutputFormat::rootevent) 
    {
      return new BDSOutputROOTEvent();
    }
  else
    {return new BDSOutputNone();} // absolute default - should not reach this
}
