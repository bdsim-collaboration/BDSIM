#ifndef BDSINTEGRATORQUADRUPOLE_H
#define BDSINTEGRATORQUADRUPOLE_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;
class BDSStep;

/**
 * @brief Integrator that ignores the field and uses the analytical solution to a quadrupole.
 * 
 * Analytical solution to a quadrupole field. This integrator will use the analytical solution
 * for a quadrupole (matrix) to transport a particle along a given step length. This will only
 * do so for particles that are considered paraxial. For particles that don't meet this criteria
 * the backupStepper from BDSIntegratorMag is used to integrate through the quadrupolar
 * field. This ensures the integrator functions correctly with particles with large transverse
 * momenta, or even ones that are travelling backwards such as secondaries.
 * 
 */

class BDSIntegratorQuadrupole: public BDSIntegratorMag
{
public:
  BDSIntegratorQuadrupole(BDSMagnetStrength const* strength,
			  G4double                 brho,
			  G4Mag_EqRhs*             eqOfMIn,
			  G4double minimumRadiusOfCurvatureIn);
  
  virtual ~BDSIntegratorQuadrupole(){;}

  /// Check if the quadrupole has finite strength and use drift if not. If finite strength,
  /// convert to local curvilinear coordiantes and check for paraxial approximation. If paraxial,
  /// use thick quadrupole matrix for transport, else use the G4ClassicalRK4 backup stepper.
  virtual void Stepper(const G4double y[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

protected:

  virtual BDSStep GlobalToCurvilinear(G4ThreeVector position,
				      G4ThreeVector unitMomentum,
				      G4double      h,
				      G4bool        useCurvilinearWorld);

  virtual BDSStep CurvilinearToGlobal(G4ThreeVector localPosition,
				      G4ThreeVector localMomentum,
				      G4bool        useCurvilinearWorld);
  
private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorQuadrupole();
  
  /// B Field Gradient
  G4double bPrime;

  /// Minimum radius of curvature in field. Below this, the backup integrator is used.
  /// Rarely, a particle may meet paraxial requirements but experience near spiralling
  /// in which case the thick matrix won't produce accurate or numerically stable results.
  G4double minimumRadiusOfCurvature;
};

#endif
