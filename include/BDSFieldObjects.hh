#ifndef BDSFIELDOBJECTS_H
#define BDSFIELDOBJECTS_H

#include "globals.hh" // geant4 types / globals

#include <vector>

class BDSFieldInfo;

class G4ChordFinder;
class G4ElectroMagneticField;
class G4EquationOfMotion;
class G4Field;
class G4FieldManager;
class G4LogicalVolume;
class G4MagInt_Driver;
class G4MagIntegratorStepper;
class G4MagneticField;

/**
 * @brief A holder for all the Geant4 field related objects.
 *
 * These are the objects that
 * are required for a magnetic field other than the  magnetic field
 * itself. If owns all objects apart from the G4MagneticField. This
 * is used purely to construct the other objects or as a reference
 * to which field they're based on.
 * 
 * @author Laurie Nevay
 */

class BDSFieldObjects
{
public:
  /// A field is required to build the required objects to manage and use it.
  BDSFieldObjects(const BDSFieldInfo*     infoIn,
		  G4Field*                fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn,
		  G4ChordFinder*          chordFinderIn,
		  G4FieldManager*         fieldManagerIn);

  /// Alternative constructor that constructs G4ChordFinder and G4FieldManager
  /// for convenience. The field manager uses delta intersection, epsilon min / max,
  /// and delta one step from BDSGlobalConstants as defaults for field propagation
  /// accuracy.
  BDSFieldObjects(const BDSFieldInfo*     infoIn,
		  G4MagneticField*        fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn);

  /// Alternative constructor for the special case of an electro-magnetic field
  /// which doesn't follow the same hierarchy of classes as magnetic fields.
  BDSFieldObjects(const BDSFieldInfo*     infoIn,
		  G4ElectroMagneticField* fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn);

  /// Destructor deletes all objects apart from the magnetic field
  ~BDSFieldObjects();

  ///@{ Acessor.
  inline const BDSFieldInfo*     GetInfo()             const {return info;}
  inline G4Field*                GetField()            const {return field;}
  inline G4EquationOfMotion*     GetEquationOfMotion() const {return equationOfMotion;}
  inline G4MagIntegratorStepper* GetIntegrator()       const {return magIntegratorStepper;}
  inline G4ChordFinder*          GetChordFinder()      const {return chordFinder;}
  inline G4FieldManager*         GetFieldManager()     const {return fieldManager;}
  inline G4MagInt_Driver*        GetMagIntDriver()     const {return magIntDriver;}
  ///@}

  /// Interface to easily attach to logical volume.
  void AttachToVolume(G4LogicalVolume* volume,
		      G4bool penetrateToDaughterVolumes = true);

  /// Interface to easily attach to logical volumes.
  void AttachToVolume(std::vector<G4LogicalVolume*> volumes,
		      G4bool penetrateToDaughterVolumes = true);
  
private:
  /// Private default constructor to force use of non-default constructor
  BDSFieldObjects();

  /// Private copy constructor to avoid shallow pointer copy and possible double deletion.
  /// Could possibly implement this with the info and the field factory.
  BDSFieldObjects(const BDSFieldObjects& other);

  /// The complete information required to build this field.
  const BDSFieldInfo* info;
  
  /// Reference to field this instance is based on.
  G4Field* field;
  
  /// Equation of motion, typically G4Mag_UsualEqRhs instance
  G4EquationOfMotion* equationOfMotion;

  /// Stepper, selectable depending on smoothness of the field etc.
  G4MagIntegratorStepper* magIntegratorStepper;

  /// Chord manager
  G4ChordFinder* chordFinder;

  /// Field manager
  G4FieldManager* fieldManager;

  /// EM field integrator driver (optional) - only for EM fields.
  G4MagInt_Driver* magIntDriver;
};

#endif
