#ifndef BDSINTEGRATORDIPOLEFRINGE_H
#define BDSINTEGRATORDIPOLEFRINGE_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDipole2.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution for a dipole kick.
 *
 * @author Will Shields 
 */

class BDSIntegratorDipoleFringe: public BDSIntegratorDipole2, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorDipoleFringe(BDSMagnetStrength const* strength,
			    G4Mag_EqRhs*             eqOfMIn,
			    G4double                 minimumRadiusOfCurvature);
  
  virtual ~BDSIntegratorDipoleFringe(){;}

  /// The stepper for integration. Uses BDSIntegratorDipole2::Stepper and then adds
  /// a kick in yp in curvilinear coordinates.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double stepLength,
		       G4double       yOut[],
		       G4double       yErr[]);
  
private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleFringe() = delete;
  
  /// Poleface rotation angle
  const G4double polefaceAngle;
  /// Fringe field correction term
  const G4double fringeCorr;
};

#endif
