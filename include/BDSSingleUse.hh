#ifndef BDSSINGLEUSE
#define BDSSINGLEUSE

#include "globals.hh"

/**
 * @brief Class to provide single use functionality.
 * 
 * @author Laurie Nevay
 */

class BDSSingleUse
{
public:
  BDSSingleUse(){;}
  virtual ~BDSSingleUse(){;}

  /// Get whether this instance has been activated.
  inline G4bool Activated() const {return activated;}

  /// Flag this instance as activated for later querying.
  inline void   SetActivated() {activated = true;}

protected:
  G4bool activated = false;
};

#endif
