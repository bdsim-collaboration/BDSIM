#ifndef BDSFIELDFACTORY_H
#define BDSFIELDFACTORY_H

#include "BDSFieldObjects.hh"
#include "BDSFieldType.hh"
#include "BDSGeometry.hh"
#include "BDSMagFieldMesh.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"

#include "globals.hh" // geant4 globals / types
#include "G4AffineTransform.hh"
#include "G4EquationOfMotion.hh"
#include "G4Field.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

class BDSGeometry;

/**
 * @brief Factory that produces fields and their associated objects.
 * 
 * Field objects are created according to a BDSFieldType and the associated
 * and required Geant4 objects to properly implement a field. These are packaged
 * together in one object. This factory does not own any of its products. Construction
 * follows in this order:
 *
 * 1 field
 * 2 equation of motion (based on field object)
 * 3 integrator
 * 4 chord finder
 * 5 field manager
 * 6 package it up
 * 
 * @author L. Deacon & L. Nevay
 */

class BDSFieldFactory
{
public:
  /// Public accessor method for singleton pattern.
  static BDSFieldFactory* Instance();
  
  ~BDSFieldFactory();

  /// Create a field based on a mesh of coordinates with field 3 vectors. This only
  /// produces the field object itself.
  BDSMagFieldMesh* CreateMagneticField(G4String      formatAndFilePath,
				       G4ThreeVector offsetIn   = G4ThreeVector(0,0,0),
				       BDSGeometry*  geometryIn = nullptr);

  /// Create a pure magnetic field as described by an equation, such as a quadupole or
  /// dipole field.  All associated objects are created and packaged together.
  BDSFieldObjects* CreateFieldEquation(BDSFieldType       type,
				       BDSMagnetStrength* strength,
				       G4double           brho);

  /// Create a pure magnetic field as described by an equation, such as a quadupole or
  /// dipole field.  All associated objects are created and packaged together.
  BDSFieldObjects* CreateFieldMagEquation(BDSMagnetType      type,
					  BDSMagnetStrength* strength,
					  G4double           brho);

  /// Create a field from a mesh of coordinates with field 3 vectors. All associated
  /// objects are created and packaged together.
  BDSFieldObjects* CreateFieldMesh(BDSFieldType       type,
				   G4String           filename,
				   G4ThreeVector      localOffset = G4ThreeVector(0,0,0),
				   G4AffineTransform* transform = nullptr);
  
private:
  /// Private default constructor as singleton class.
  BDSFieldFactory();

  /// Instance - singleton patter
  static BDSFieldFactory* instance;
  
  G4ThreeVector offset;
  BDSFieldType  format;
  G4String      fileName;
  BDSGeometry*  geometry;
  G4double      cacheLength;

  ///@{ Variable to allow different functions to access different parts during construction
  G4Field*                field;
  G4MagneticField*        bField;
  G4EquationOfMotion*     eqOfMotion;
  /// B Fields require at least this level in the inheritance - use G4Mag_EqRhs instead
  /// of G4EquationOfMotion for b fields
  G4Mag_EqRhs*            bEqOfMotion;
  G4MagIntegratorStepper* integrator;
  BDSFieldObjects*        completeField;
  ///@}
  
  /// Common construction of building field objects
  void CommonConstructor();

  ///@{ Delegate function to construct the specific classes
  void CreateSolenoid(BDSMagnetStrength*   strength, G4double brho);
  void CreateDipole(BDSMagnetStrength*     strength, G4double brho);
  void CreateQuadrupole(BDSMagnetStrength* strength, G4double brho);
  void CreateSextupole(BDSMagnetStrength*  strength, G4double brho);
  void CreateOctupole(BDSMagnetStrength*   strength, G4double brho);
  void CreateDecapole(BDSMagnetStrength*   strength, G4double brho);
  void CreateMultipole(BDSMagnetStrength*  strength, G4double brho);
  void CreateKicker(BDSMagnetStrength*  strength, G4double brho, G4bool isVertical);
  ///@}
  
  /// Splits the G4String member variable formatAndName on the ":" character.
  /// Whatever is before is taken as the fromat string and whatever is after is
  /// taken as the filepath.
  void ParseFormatAndFilename();

  /// Reset all pointers to nullptr that are temporarily used during construction
  /// to avoid mistaken contamination between uses of the factory
  void CleanUp();

  BDSMagFieldMesh* CreateMagField3D();
  BDSMagFieldMesh* CreateMagFieldXY();
  BDSMagFieldMesh* CreateMagFieldLCDD();
  BDSMagFieldMesh* CreateMagFieldSQL();
};
#endif
