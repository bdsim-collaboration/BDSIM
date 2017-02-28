#ifndef BDSINTEGRATOR_H
#define BDSINTEGRATOR_H

#include "BDSAuxiliaryNavigator.hh"

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

class BDSIntegrator: public G4MagIntegratorStepper
{
public:
  BDSIntegrator(G4Mag_EqRhs* eqOfMIn,
		G4int        nVariablesIn);
  
  virtual ~BDSIntegrator();

  /// Estimate maximum distance of curved solution and chord.
  inline virtual G4double DistChord() const {return distChordPrivate;}

  /// Geant4 requires that the integrator order must be supplied by the derived class.
  inline virtual G4int IntegratorOrder() const {return 2;}
  
protected:
  /// Advance as drift (for when field strength is very low)
  void AdvanceDrift(const G4double yIn[],
		    const G4ThreeVector& GlobalP,
		    const G4double h,
		    G4double yOut[]);

  /// Advance chord by quadratic approximation
  void AdvanceChord(const G4double h,
		    G4ThreeVector& LocalR,
		    G4ThreeVector& LocalRp,
		    const G4ThreeVector& LocalRpp);

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

  inline void SetDistChord(G4double distChordIn) {distChordPrivate = distChordIn;}

private:
  /// Private default constructor to force use of specific constructor
  BDSIntegrator();

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChordPrivate;
};

#endif
