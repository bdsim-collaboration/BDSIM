#ifndef BDSINTEGRATORMAG_H
#define BDSINTEGRATORMAG_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDrift.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4MagIntegratorStepper.hh"

class G4Mag_EqRhs;

/**
 * @brief Common functionality to BDSIM integrators.
 *
 * This provides a general 4th order Runge Kutta integrator that can be used
 * by the derived class if the coordinates are non-paraxial for example.
 *
 * The derived class must also satisfy G4MagIntegratorStepper.hh's virtual method
 * Stepper.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorMag: public G4MagIntegratorStepper, public BDSIntegratorDrift, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorMag(G4Mag_EqRhs* eqOfMIn,
		   G4int        nVariablesIn);
  
  virtual ~BDSIntegratorMag();

  /// Estimate maximum distance of curved solution and chord.
  inline virtual G4double DistChord() const {return distChordPrivate;}

  /// Geant4 requires that the integrator order must be supplied by the derived class.
  inline virtual G4int IntegratorOrder() const {return 2;}

  /// Cache of thin element length to know maximum possible length scale step
  /// for coordinate lookup.
  static G4double thinElementLength;

protected:
  /// Convert final local position and direction to global frame. Allow
    /// scaling of momentum in case localMom is a unit vector
  void ConvertToGlobal(const G4ThreeVector &localPos,
                       const G4ThreeVector &localMom,
                       G4double             yOut[],
                       const G4double       momScaling = 1.0);

  /// Setter for distChord to private member.
  inline void SetDistChord(G4double distChordIn) {distChordPrivate = distChordIn;}
  
  /// Keep a reference to the underlying equation of motion, but through a higher
  /// level pointer than G4EquationOfMotion* so we can use the correct methods. This
  /// class doesn't own this.
  G4Mag_EqRhs* eqOfM;
  
  /// Cache of the number of variables.
  const G4int nVariables;

  /// General integrator that can be used as a backup if the particle momentum is
  /// outside the (transverse) momentum range applicable for the integration scheme
  /// used by the derived integrator.
  G4MagIntegratorStepper* backupStepper;

  /// Convert to curvilinear coordinates.
  BDSStep GlobalToCurvilinear(BDSMagnetStrength const* strength,
                                      G4ThreeVector position,
                                      G4ThreeVector unitMomentum,
                                      G4double      h,
                                      G4bool        useCurvilinearWorld);

  BDSStep GlobalToCurvilinear(G4ThreeVector position,
                                      G4ThreeVector unitMomentum,
                                      G4double      h,
                                      G4bool        useCurvilinearWorld);

  BDSStep CurvilinearToGlobal(G4ThreeVector localPosition,
                                      G4ThreeVector localMomentum,
                                      G4bool        useCurvilinearWorld);

private:
  /// Private default constructor to force use of specific constructor
  BDSIntegratorMag() = delete;

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChordPrivate;
};

#endif
