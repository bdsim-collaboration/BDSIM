#ifndef BDSOUTPUTFACTORY_H
#define BDSOUTPUTFACTORY_H

#include "BDSOutputFormat.hh"

class BDSOutput;

/**
 * @brief factory class for BDSOutput
 * 
 * @author Jochem Snuverink
 */

class BDSOutputFactory {

public:
  /// factory method
  static BDSOutput* CreateOutput(BDSOutputFormat format);
};

#endif
