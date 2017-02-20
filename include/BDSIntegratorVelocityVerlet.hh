#ifndef BDSINTEGRATORVELOCITYVERLET_H
#define BDSINTEGRATORVELOCITYVERLET_H

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"

class G4MagIntegratorStepper;
class G4Mag_EqRhs;

/**
 * @brief Velocity-Verlet integator.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorVelocityVerlet: public G4MagIntegratorStepper
{
public:
  BDSIntegratorVelocityVerlet(G4Mag_EqRhs* eqOfMIn);

  virtual ~BDSIntegratorVelocityVerlet();

  /// Calculate output coordinates.
  virtual void Stepper(const G4double yIn[],     ///< Input coordinates x,y,z,px,py,pz,t
		       const G4double dydx[],    ///< Partial differentials at yInput
		       G4double       stepLength,///< Length of trajectory to calculate
		       G4double       yOut[],    ///< Output array
		       G4double       yErr[]);   ///< Output error array

  virtual G4double DistChord() const {return distChord;}

  virtual G4int IntegratorOrder() const {return 2;}

private:
  /// Private default constructor to force use of provided one.
  BDSIntegratorVelocityVerlet() = delete;

  /// General integrator that can be used as a backup if the particle momentum is
  /// outside the (transverse) momentum range applicable for the integration scheme
  /// used by the derived integrator.
  G4MagIntegratorStepper* backupIntegrator;

  G4double distChord;
};

#endif
