#include "TRK.hh"

#include "BDSOutputASCII.hh"
#include "BDSOutputROOT.hh"
#include "BDSExecOptions.hh"
#include "BDSOutputFormat.hh"

//This is based on a section from the bdsim.cc main, but put in a function
BDSOutputBase* TRK::InitialiseOutput() {
  if (BDSExecOptions::Instance()->GetOutputFormat() == BDSOutputFormat::_ASCII) {
    return new BDSOutputASCII();
}
/*
  else if (BDSExecOptions::Instance()->GetOutputFormat() == BDSOutputFormat::_ROOT) {
#ifdef USE_ROOT
    return new BDSOutputROOT();
#else
    return new BDSOutputASCII();
#endif
  }
//If I include this section of code, I get a linker error of duplicate symbol _main...
//don't need root output just now.
*/
  else {
    return new BDSOutputASCII();
  }
}
