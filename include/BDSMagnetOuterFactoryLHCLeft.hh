#ifndef BDSMAGNETOUTERFACTORYLHCLEFT_H
#define BDSMAGNETOUTERFACTORYLHCLEFT_H

#include "BDSMagnetOuterFactoryLHC.hh"

/**
 * @brief LHC outer magnet geometry offset to the left
 * 
 * This factory gets everything from the LHC factory and 
 * simply has it's own singleton accessor
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSMagnetOuterFactoryLHCLeft: public BDSMagnetOuterFactoryLHC
{
public:
  static BDSMagnetOuterFactoryLHCLeft* Instance(); /// singleton pattern
  
  ~BDSMagnetOuterFactoryLHCLeft();

private:
  BDSMagnetOuterFactoryLHCLeft(); //private constructor as singleton
  static BDSMagnetOuterFactoryLHCLeft* _instance;
};

#endif
