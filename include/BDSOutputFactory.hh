#ifndef BDSOUTPUTFACTORY_H
#define BDSOUTPUTFACTORY_H

#include "BDSOutputType.hh"

class BDSOutput;

/**
 * @brief factory class for BDSOutput
 * 
 * @author Jochem Snuverink
 */

class BDSOutputFactory
{
public:
  /// factory method
  static BDSOutput* CreateOutput(BDSOutputType format,
				 G4String      fileName,
				 G4int         fileNumberOffset = -1);
};

#endif
