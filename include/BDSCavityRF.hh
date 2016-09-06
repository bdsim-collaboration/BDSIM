#ifndef BDSCAVITYRF_H
#define BDSCAVITYRF_H

#include "BDSCavity.hh"

#include "globals.hh" // geant4 globals / types

class BDSCavityInfo;

/** 
 * @brief RF Cavity class
 *  
 * @author Stuart Walker
 */
class BDSCavityRF: public BDSCavity
{
public:

  BDSCavityRF(G4String       name,
	      G4double       length,
	      G4double       fieldAmplitude,
	      BDSCavityInfo* cavityInfo);
    
  /// Creates field objects - doesn't nothing by default and derived classes can override.
  virtual void BuildField();

  /// Attach the created field to the vacuum logical volume - only if field exists. Does
  /// nothing by default as no field by default.
  virtual void AttachField();
};
#endif
