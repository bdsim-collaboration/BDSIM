#ifndef BDSINTEGRATORDECAPOLE_H
#define BDSINTEGRATORDECAPOLE_H

#include "BDSIntegratorEulerOld.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for Decapolar field.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorDecapole: public BDSIntegratorEulerOld
{
public:
  BDSIntegratorDecapole(BDSMagnetStrength const* strength,
			G4double                 brho,
			G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorDecapole(){;}

protected:
  /// Calculate the new particle coordinates.
  virtual void AdvanceHelix(const G4double yIn[],
			    G4double       h,
			    G4double       yDec[]);
  
private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDecapole() = delete;

  /// 4th derivative of magnetic field
  G4double bQuadruplePrime;
};

#endif
