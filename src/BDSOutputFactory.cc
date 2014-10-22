#include "BDSOutputFactory.hh"

#include "BDSOutputBase.hh"
#include "BDSOutputASCII.hh"
#include "BDSOutputROOT.hh"

BDSOutputBase* BDSOutputFactory::createOutput(BDSOutputFormat format) {
  if (format == BDSOutputFormat::_ASCII) {
    return new BDSOutputASCII();
  }
  else if (format == BDSOutputFormat::_ROOT) {
#ifdef USE_ROOT
    return new BDSOutputROOT();
#endif
  }
  // should not happen, but to prevent compiler warning
  return new BDSOutputASCII();
}
