#ifndef BDSINTEGRATORSYMPLECTICEULER_H
#define BDSINTEGRATORSYMPLECTICEULER_H

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"

class G4MagIntegratorStepper;
class G4Mag_EqRhs;

/**
 * @brief 2nd order symplectic Euler integration.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorSymplecticEuler: public G4MagIntegratorStepper
{
public:
  BDSIntegratorSymplecticEuler(G4Mag_EqRhs* eqOfMIn);

  virtual ~BDSIntegratorSymplecticEuler();

  /// Calculate output coordinates.
  virtual void Stepper(const G4double yIn[],     ///< Input coordinates x,y,z,px,py,pz,t
		       const G4double dydx[],    ///< Partial differentials at yInput
		       G4double       stepLength,///< Length of trajectory to calculate
		       G4double       yOut[],    ///< Output array
		       G4double       yErr[]);   ///< Output error array

  /// Return the most recently calculated distance from the chord. The estimated
  /// maximum distance between the true arced path and a chord between the start
  /// and finish point.
  virtual G4double DistChord() const {return distChord;}

  /// Order required by Geant4.  2nd order.  Although with higher order accuracy.
  virtual G4int IntegratorOrder() const {return 2;}

private:
  /// Private default constructor to force use of provided one.
  BDSIntegratorSymplecticEuler() = delete;
  
  void SimpleStepper(const G4double yIn[],     ///< Input coordinates x,y,z,px,py,pz,t
		     const G4double dydx[],    ///< Partial differentials at yInput
		     G4double       stepLength,///< Length of trajectory to calculate
		     G4double       yOut[],    ///< Output array
		     G4double       yErr[]);

  /// General integrator that can be used as a backup if the particle momentum is
  /// outside the (transverse) momentum range applicable for the integration scheme
  /// used by the derived integrator.
  G4MagIntegratorStepper* backupIntegrator;

  /// The distance between a straight line between start and finish and the maximum
  /// extend of the arced true path through the field.
  G4double distChord;
};

#endif
