#ifndef BDSINTEGRATORSEXTUPOLE_H
#define BDSINTEGRATORSEXTUPOLE_H

#include "BDSIntegratorEulerOld.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for sextupole field.
 */

class BDSIntegratorSextupole: public BDSIntegratorEulerOld
{
public:
  BDSIntegratorSextupole(BDSMagnetStrength const* strength,
			 G4double                 brho,
			 G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorSextupole(){;}
  
protected:
  /// Calculate the new particle coordinates.
  virtual void AdvanceHelix(const G4double yIn[],
			    G4double       h,
			    G4double       yOut[]);
  
private:
  BDSIntegratorSextupole() = delete;

  /// 2nd derivative of the field
  G4double bDoublePrime;
};

#endif
