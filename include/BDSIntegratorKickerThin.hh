#ifndef BDSINTEGRATORKICKERTHIN_H
#define BDSINTEGRATORKICKERTHIN_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator for thin h or v kick.
 * 
 * This adds a fraction of the total momentum to the horizontal
 * or vertical (in curvilinear coordinates) as defined by hkick
 * and vkick respectively. The delta is subtracted from the z
 * component to conserve momentum.
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

  /// @{ Cache of variable.
  const G4double hkick;
  const G4double vkick;
  const G4double brho;
  /// @}

  /// Cache of whether input parameters are 0 and therefore whether to kick at all.
  G4bool zeroStrength;

  /// Cache of thin element length from global constants. Initialised via check
  /// on unphysical -1 value as global constants doesn't exist at compile time.
  static G4double thinElementLength;
};

#endif
