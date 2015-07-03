#include "BDSOutputFactory.hh"

#include "BDSOutputFormat.hh"
#include "BDSOutputBase.hh"
#include "BDSOutputASCII.hh"
#include "BDSOutputNone.hh"
#include "BDSOutputROOT.hh"
#include "BDSOutputVector.hh"

#include "BDSDebug.hh"

BDSOutputBase* BDSOutputFactory::createOutput(BDSOutputFormat format)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format = " << format << G4endl;
#endif
  if (format == BDSOutputFormat::_NONE)
    {return new BDSOutputNone();}
  
  if (format == BDSOutputFormat::_COMBINED)
    {
      BDSOutputVector* combinedOutput = new BDSOutputVector();
      combinedOutput->Add(new BDSOutputASCII());
#ifdef USE_ROOT
      combinedOutput->Add(new BDSOutputROOT());
#endif
      return combinedOutput;
    }
  
  else if (format == BDSOutputFormat::_ASCII)
    {return new BDSOutputASCII();}
  
  else if (format == BDSOutputFormat::_ROOT)
    {
#ifdef USE_ROOT
    return new BDSOutputROOT();
#else
    G4cout << __METHOD_NAME__ << "warning: root output selected but BDSIM not built with root support" << G4endl;
    G4cout << __METHOD_NAME__ << "using ASCII by default" << G4endl;
    return new BDSOutputASCII();
#endif
    }
  else
    {return NULL;}
}
