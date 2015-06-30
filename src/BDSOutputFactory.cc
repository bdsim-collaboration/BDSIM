#include "BDSOutputFactory.hh"

#include "BDSOutputFormat.hh"
#include "BDSOutputBase.hh"
#include "BDSOutputASCII.hh"
#include "BDSOutputNone.hh"
#include "BDSOutputROOT.hh"
#include "BDSOutputVector.hh"

BDSOutputBase* BDSOutputFactory::createOutput(BDSOutputFormat format) {
  if (format == BDSOutputFormat::_NONE) {
    return new BDSOutputNone();
  }
  else if (format == BDSOutputFormat::_ASCII) {
    return new BDSOutputASCII();
  }
  else if (format == BDSOutputFormat::_ROOT) {
#ifdef USE_ROOT
    return new BDSOutputROOT();
#endif
  }
//   else if (format == BDSOutputFormat::_COMBINED) {
//     BDSOutputVector* combinedOutput = new BDSOutputVector();
//     combinedOutput->Add(new BDSOutputASCII());
// #ifdef USE_ROOT
//     combinedOutput->Add(new BDSOutputROOT());
// #endif
//     bdsOutput = combinedOutput;
//   }
  // should not happen, but to prevent compiler warning
  return new BDSOutputASCII();
}
