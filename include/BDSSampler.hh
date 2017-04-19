#ifndef BDSSAMPLER_H
#define BDSSAMPLER_H

#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Base class and registry of sampler instances.
 * 
 * @author Laurie Nevay
 */

class BDSSampler: public BDSGeometryComponent
{
public:
  explicit BDSSampler(G4String nameIn);
  virtual ~BDSSampler(){;}

  /// Return the name of this sampler.
  inline G4String GetName() const {return name;}
  
protected:
  /// Common construction tasks such as creating a logical volume from the solid
  /// and visualisation options.
  void CommonConstruction();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSSampler() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSSampler& operator=(const BDSSampler&) = delete;
  BDSSampler(BDSSampler&) = delete;
  /// @}
  
  /// Name of this sampler
  G4String name;
};

#endif
