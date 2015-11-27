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
#ifdef USE_ROOT
      combinedOutput->Add(new BDSOutputROOT());
#endif
      return combinedOutput;
    }
  
  else if (format == BDSOutputFormat::ascii)
    {return new BDSOutputASCII();}
  
  else if (format == BDSOutputFormat::root)
    {
#ifdef USE_ROOT
    return new BDSOutputROOT();
#else
    G4cout << __METHOD_NAME__ << "warning: root output selected but BDSIM not built with root support" << G4endl;
    G4cout << __METHOD_NAME__ << "using ASCII by default" << G4endl;
    return new BDSOutputASCII();
#endif
    }
  else if (format == BDSOutputFormat::rootdetailed) 
    {
#ifdef USE_ROOT
      return new BDSOutputROOTDetailed();
#else
    G4cout << __METHOD_NAME__ << "warning: root output selected but BDSIM not built with root support" << G4endl;
    G4cout << __METHOD_NAME__ << "using ASCII by default" << G4endl;
    return new BDSOutputASCII();
#endif
    }
  else if (format == BDSOutputFormat::rootevent) 
    {
#ifdef USE_ROOT
      return new BDSOutputROOTEvent();
#else
    G4cout << __METHOD_NAME__ << "warning: root output selected but BDSIM not built with root support" << G4endl;
    G4cout << __METHOD_NAME__ << "using ASCII by default" << G4endl;
    return new BDSOutputASCII();
#endif
    }
  else
    {return new BDSOutputNone();} // absolute default - should not reach this
}
