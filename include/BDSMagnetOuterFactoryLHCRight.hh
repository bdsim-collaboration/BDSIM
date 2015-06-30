#ifndef BDSMAGNETOUTERFACTORYLHCRIGHT_H
#define BDSMAGNETOUTERFACTORYLHCRIGHT_H

#include "BDSMagnetOuterFactoryLHC.hh"

/**
 * @brief LHC outer magnet geometry offset to the right
 * 
 * This factory gets everything from the LHC factory and 
 * simply has it's own singleton accessor
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSMagnetOuterFactoryLHCRight: public BDSMagnetOuterFactoryLHC
{
public:
  static BDSMagnetOuterFactoryLHCRight* Instance(); /// singleton pattern
  
  ~BDSMagnetOuterFactoryLHCRight();

private:
  BDSMagnetOuterFactoryLHCRight(); //private constructor as singleton
  static BDSMagnetOuterFactoryLHCRight* _instance;
};

#endif
