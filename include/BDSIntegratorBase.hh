#ifndef BDSINTEGRATORBASE_H
#define BDSINTEGRATORBASE_H

#include "BDSAuxiliaryNavigator.hh"

#include "globals.hh" // geant4 types / globals
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"

/**
 * @brief Common functionality to BDSIM integrators.
 * 
 * This is a base class for BDSIM provided integrators that ignore the true 
 * field value supplied to the integrator from Geant4 and simply integrate the
 * the motion based on the design rigidity and a strength parameter.
 * 
 * This also provides a general 4th order Runge Kutta integrator that can be used
 * by the derived class if the coordinates are non-parraxial for example.
 *
 * The derived class must also satisfy G4MagIntegratorStepper.hh's virtual method
 * Stepper.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorBase: public G4MagIntegratorStepper, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorBase(G4Mag_EqRhs* eqOfMIn,
		    G4int        nVariablesIn);
  
  virtual ~BDSIntegratorBase();

  /// Estimate maximum distance of curved solution and chord.
  inline G4double DistChord() const {return distChord;}

  /// Geant4 requires that the integrator order must be supplied by the derived class.
  inline virtual G4int IntegratorOrder() const {return 2;}
  
protected:
  /// The equation of motion object that this integrator is constructed w.r.t. This is
  /// used to get fPtrMagEqOfMot->FCof()/CLHEP::c_light, which is the charge of the particle
  /// being stepped.
  G4Mag_EqRhs* eqOfM;

  /// Number of variables this can integrate over.
  const G4int nVariables;

  /// General integrator that can be used as a backup if the particle momentum is
  /// outside the (transverse) momentum range applicable for the integration scheme
  /// used by the derived integrator.
  G4MagIntegratorStepper* backupStepper;

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChord;

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

  /// Convert final local position and direction to global frame
  void ConvertToGlobal(const G4ThreeVector& LocalR,
		       const G4ThreeVector& LocalRp,
		       const G4double InitMag,
		       G4double yOut[]);

private:
  /// Private default constructor to force use of specific constructor
  BDSIntegratorBase();
};

#endif
