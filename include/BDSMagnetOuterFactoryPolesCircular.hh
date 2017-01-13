#ifndef BDSMAGNETOUTERFACTORYPOLESCIRCULAR_H
#define BDSMAGNETOUTERFACTORYPOLESCIRCULAR_H

#include "BDSMagnetOuterFactoryPolesBase.hh"

/**
 * @brief Factory class for outer volume of magnets. Produces magnets
 * with 2N-poles around the beampipe with a circular yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay
 */


class BDSMagnetOuterFactoryPolesCircular: public BDSMagnetOuterFactoryPolesBase
{
public:
  /// Singleton accessor.
  static BDSMagnetOuterFactoryPolesCircular* Instance();
  
  virtual ~BDSMagnetOuterFactoryPolesCircular();

private:
  /// Private constructor as singleton - nothing special here - all in parent class
  BDSMagnetOuterFactoryPolesCircular(){;};

  /// Singleton instance.
  static BDSMagnetOuterFactoryPolesCircular* _instance;
};

#endif
