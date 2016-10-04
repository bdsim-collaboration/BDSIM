#ifndef BDSBunchFactory_h
#define BDSBunchFactory_h

#include "BDSOutputFormat.hh"

class BDSOutputBase;

/**
 * @brief factory class for BDSOutput
 * 
 * @author Jochem Snuverink
 */

class BDSOutputFactory {

public:
  /// factory method
  static BDSOutputBase* CreateOutput(BDSOutputFormat format);
};

#endif
