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
  BDSSampler(G4String      nameIn);
  virtual ~BDSSampler(){;}

  /// Return the name of this sampler.
  inline G4String GetName() const;
  
protected:
  /// Common construction tasks such as creating a logical volume from the solid
  /// and visualisation options.
  void CommonConstruction();

private:  
  /// Name of this sampler
  G4String name;
};

inline G4String BDSSampler::GetName() const
{return name;}

#endif
