#ifndef BDSMAGNETICFIELDOBJECTS_H
#define BDSMAGNETICFIELDOBJECTS_H

/**
 * @brief A holder for all the Geant4 field related objects that
 * are required for a magnetic field other than the  magnetic field
 * itself.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class G4EquationOfMotion;

class BDSMagneticFieldObjects
{
public:
  /// A magnetic field is required to build the required objects to manage
  /// and use it.
  BDSMagneticFieldObjects(G4MagneticField*        magneticField,
			  G4EquationOfMotion*     equationOfMotionIn,
			  G4MagIntegratorStepper* magIntegratorStepperIn,
			  G4ChordManager*         chordManagerIn,
			  G4FieldManager*         fieldManagerIn)
			  
  ~BDSMagneticFieldObjects();

private:
  /// Private default constructor to force use of non-default constructor
  BDSMagneticFieldObjects();
  
  /// Equation of motion, typically G4Mag_UsualEqRhs instance
  G4EquationOfMotion* equationOfMotion;

  /// Stepper, selectable depending on smoothness of the field etc.
  G4MagIntegratorStepper* magIntegratorStepper;

  /// Chord manager
  G4ChordManager* chordManager;

  /// Field manager
  G4FieldManager* fieldManager;
}
