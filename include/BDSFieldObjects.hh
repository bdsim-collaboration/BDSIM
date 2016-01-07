#ifndef BDSFIELDOBJECTS_H
#define BDSFIELDOBJECTS_H

/**
 * @brief A holder for all the Geant4 field related objects that
 * are required for a magnetic field other than the  magnetic field
 * itself. If owns all objects apart from the G4MagneticField. This
 * is used purely to construct the other objects or as a reference
 * to which field they're based on.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class G4ChordFinder;
class G4EquationOfMotion;
class G4FieldManager;
class G4MagIntegratorStepper;
class G4MagneticField;

class BDSFieldObjects
{
public:
  /// A magnetic field is required to build the required objects to manage
  /// and use it.
  BDSFieldObjects(G4MagneticField*        magneticFieldIn,
		     G4EquationOfMotion*     equationOfMotionIn,
		     G4MagIntegratorStepper* magIntegratorStepperIn,
		     G4ChordFinder*          chordFinderIn,
		     G4FieldManager*         fieldManagerIn);

  /// Alternative constructor that constructs G4ChordFinder and G4FieldManager
  /// for convenience. The field manager uses delta intersection, epsilon min / max,
  /// and delta one step from BDSGlobalConstants as defaults for field propagation
  /// accuracy.
  BDSFieldObjects(G4MagneticField*        magneticFieldIn,
		     G4EquationOfMotion*     equationOfMotionIn,
		     G4MagIntegratorStepper* magIntegratorStepperIn);

  /// Destructor deletes all objects apart from the magnetic field
  ~BDSFieldObjects();

private:
  /// Private default constructor to force use of non-default constructor
  BDSFieldObjects();

  /// Private copy constructor to avoid shallow pointer copy and possible double deletion
  BDSFieldObjects(const BDSFieldObjects& other);

  /// Reference to magnetic field this instance is based on (doesn't own it)
  G4MagneticField* magneticField;
  
  /// Equation of motion, typically G4Mag_UsualEqRhs instance
  G4EquationOfMotion* equationOfMotion;

  /// Stepper, selectable depending on smoothness of the field etc.
  G4MagIntegratorStepper* magIntegratorStepper;

  /// Chord manager
  G4ChordFinder*  chordFinder;

  /// Field manager
  G4FieldManager* fieldManager;
};

#endif
