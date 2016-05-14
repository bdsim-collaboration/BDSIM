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
		    G4double     nVariablesIn);
  
  virtual ~BDSIntegratorBase();

  /// Estimate maximum distance of curved solution and chord.
  inline G4double DistChord() const;

  /// Geant4 requires that the integrator order must be supplied by the derived class.
  inline virtual G4int IntegratorOrder() const;
  
protected:
  /// The equation of motion object that this integrator is constructed w.r.t. This is
  /// used to get fPtrMagEqOfMot->FCof()/CLHEP::c_light, which is the charge of the particle
  /// being stepped.
  G4Mag_EqRhs* eqOfM;

  /// Number of variables this can integrate over.
  const G4double nVariables;

  /// General integrator that can be used as a backup if the particle momentum is
  /// outside the (transverse) momentum range applicable for the integration scheme
  /// used by the derived integrator.
  G4MagIntegratorStepper* backupStepper;

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChord;

private:
  /// Private default constructor to force use of specific constructor
  BDSIntegratorBase();
};

inline G4double BDSIntegratorBase::DistChord() const
{return distChord;}

inline G4int BDSIntegratorBase::IntegratorOrder() const
{return 2;}

#endif
