#ifndef BDSBUNCHFACTORY_H
#define BDSBUNCHFACTORY_H

#include "globals.hh"

class BDSBunchInterface;

/**
 * @brief factory class for BDSBunch
 * 
 * @author Jochem Snuverink
 */

class BDSBunchFactory
{
public:
  /// factory method
  static BDSBunchInterface* createBunch(G4String distribType, G4String distribFile = "");
};

#endif
