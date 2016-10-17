#ifndef BDSCAVITYRF_H
#define BDSCAVITYRF_H

#include "BDSCavity.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Material.hh"

class BDSCavityInfo;
class BDSFieldInfo;

/**
 * @brief An RF cavity with different geometry types.
 *
 * @author Stuart Walker
 */

/** 
 * @brief RF Cavity class
 *  
 * @author Stuart Walker
 */
class BDSCavityRF: public BDSCavity
{
public:
  BDSCavityRF(G4String      name,
	      G4double      length,
	      BDSFieldInfo* vacuumFieldIn);

  virtual ~BDSCavityRF(){;}
    
  /// Creates field objects - doesn't nothing by default and derived classes can override.
  virtual void BuildField();
};
#endif
