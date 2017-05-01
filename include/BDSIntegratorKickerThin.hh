#ifndef BDSINTEGRATORKICKERTHIN_H
#define BDSINTEGRATORKICKERTHIN_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for thin h or v kick.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorKickerThin: public BDSIntegratorMag
{
public:
  BDSIntegratorKickerThin(BDSMagnetStrength const* strength,
			  G4double                 brhoIn,
			  G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorKickerThin(){;}

  /// The stepper for integration.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorKickerThin() = delete;

  /// Cache of equation of motion. This class does not own it.
  G4Mag_EqRhs* eqOfM;

  G4double hkick;
  G4double vkick;
	G4double brho;
};

#endif
