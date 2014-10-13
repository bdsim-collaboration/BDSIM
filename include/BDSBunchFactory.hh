#ifndef BDSBunchFactory_h
#define BDSBunchFactory_h

#include "globals.hh"

class BDSBunchInterface;

/**
 * @brief factory class for BDSBunch
 * 
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk>
 */

class BDSBunchFactory {

public:
  /// factory method
  static BDSBunchInterface* createBunch(G4String distribType);
};

#endif
