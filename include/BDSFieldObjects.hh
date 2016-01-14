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
class G4Field;

class BDSFieldObjects
{
public:
  /// A field is required to build the required objects to manage and use it.
  BDSFieldObjects(G4Field*                fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn,
		  G4ChordFinder*          chordFinderIn,
		  G4FieldManager*         fieldManagerIn);

  /// Alternative constructor that constructs G4ChordFinder and G4FieldManager
  /// for convenience. The field manager uses delta intersection, epsilon min / max,
  /// and delta one step from BDSGlobalConstants as defaults for field propagation
  /// accuracy.
  BDSFieldObjects(G4MagneticField*        fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn);

  /// Destructor deletes all objects apart from the magnetic field
  ~BDSFieldObjects();

  ///@{ Acessor.
  inline G4Field*                GetField()            const;
  inline G4EquationOfMotion*     GetEquationOfMotion() const;
  inline G4MagIntegratorStepper* GetIntegrator()       const;
  inline G4ChordFinder*          GetChordFinder()      const;
  inline G4FieldManager*         GetFieldManager()     const;
  ///@}
  
private:
  /// Private default constructor to force use of non-default constructor
  BDSFieldObjects();

  /// Private copy constructor to avoid shallow pointer copy and possible double deletion
  BDSFieldObjects(const BDSFieldObjects& other);

  /// Reference to field this instance is based on.
  G4Field* field;
  
  /// Equation of motion, typically G4Mag_UsualEqRhs instance
  G4EquationOfMotion* equationOfMotion;

  /// Stepper, selectable depending on smoothness of the field etc.
  G4MagIntegratorStepper* magIntegratorStepper;

  /// Chord manager
  G4ChordFinder*  chordFinder;

  /// Field manager
  G4FieldManager* fieldManager;
};

inline G4Field* BDSFieldObjects::GetField() const
{return field;}

inline G4EquationOfMotion* BDSFieldObjects::GetEquationOfMotion() const
{return equationOfMotion;}

inline G4MagIntegratorStepper* BDSFieldObjects::GetIntegrator() const
{return magIntegratorStepper;}

inline G4ChordFinder* BDSFieldObjects::GetChordFinder() const
{return chordFinder;}

inline G4FieldManager* BDSFieldObjects::GetFieldManager() const
{return fieldManager;}

#endif
