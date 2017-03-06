#ifndef BDSINTEGRATOROCTUPOLE_H
#define BDSINTEGRATOROCTUPOLE_H

#include "BDSIntegratorEulerOld.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/** 
 * @brief Integrator for octupole field.
 */

class BDSIntegratorOctupole: public BDSIntegratorEulerOld
{
public:
  BDSIntegratorOctupole(BDSMagnetStrength const* strength,
			G4double                 brho,
			G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorOctupole(){;}

protected:
  /// Calculate the new particle coordinates.
  virtual void AdvanceHelix(const G4double yIn[],
			    G4double       h,
			    G4double       yOut[]);

private:
  BDSIntegratorOctupole() = delete;

  /// 3rd differential of field.
  G4double bTriplePrime;
};

#endif
