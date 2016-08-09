#ifndef BDSMAGNETOUTERFACTORYPOLESFACETCROP_H
#define BDSMAGNETOUTERFACTORYPOLESFACETCROP_H

#include "BDSMagnetOuterFactoryPolesFacetCommon.hh"

#include "globals.hh" // geant4 globals / types

/**
 * @brief Factory class for outer volume of magnets. 
 *
 * Produces magnets
 * with 2N-poles around the beampipe with a faceted yoke - the yoke 
 * is perpendiular to the pole direction where the pole joins the yoke.
 * Between each pole there is a flat section angled between the two poles
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryPolesFacetCrop: public BDSMagnetOuterFactoryPolesFacetCommon
{
public:
  /// Singleton accessory.
  static BDSMagnetOuterFactoryPolesFacetCrop* Instance();
  
  virtual ~BDSMagnetOuterFactoryPolesFacetCrop();

protected:
  /// Calculate start angle of polyhedra based on order and segment angle.
  virtual void CalculateStartAngles();

private:
  /// Private constructor as singleton
  BDSMagnetOuterFactoryPolesFacetCrop();

  /// Singleton instance.
  static BDSMagnetOuterFactoryPolesFacetCrop* _instance;  
};

#endif
