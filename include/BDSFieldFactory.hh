#ifndef BDSFIELDFACTORY_H
#define BDSFIELDFACTORY_H

#include "BDSFieldObjects.hh"
#include "BDSFieldType.hh"
#include "BDSGeometry.hh"
#include "BDSMagFieldMesh.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 globals / types

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
  BDSFieldFactory();
  ~BDSFieldFactory();

  /// Create a field based on a mesh of coordinates with field 3 vectors. This only
  /// produces the field object itself.
  BDSMagFieldMesh* BuildMagneticField(G4String      formatAndFilePath,
				      G4ThreeVector offsetIn   = G4ThreeVector(0,0,0),
				      BDSGeometry*  geometryIn = nullptr);

  /// Create a pure magnetic field as described by an equation, such as a quadupole or
  /// dipole field.  All associated objects are created and packaged together.
  BDSFieldObjects* BuildFieldEquation(BDSFieldType       type,
				      BDSMagnetStrength* strength,
				      G4double           brho);

  /// Create a field from a mesh of coordinates with field 3 vectors. All associated
  /// objects are created and packaged together.
  BDSFieldObjects* BuildFieldMesh(BDSFieldType  type,
				  G4String      filename,
				  G4ThreeVector localOffset = G4ThreeVector(0,0,0),
				  G4Transform*  transform = nullptr);
 
private:  
  G4ThreeVector offset;
  BDSFieldType  format;
  G4String      fileName;
  BDSGeometry*  geometry;
  G4double      cacheLength;

  /// Variables to allow different functions to access different parts during construction
  G4Field*                field;
  G4EquationOfMotion*     eqOfM;
  G4MagIntegratorStepper* integrator;
  BDSFieldObjects*        completeField;

  /// Common construction of building field objects
  void CommonConstructor();

  ///@{ Delegate function to construct the specific classes
  void BuildSolenoid(BDSMagnetStrength*   strength, G4double brho);
  void BuildDipole(BDSMagnetStrength*     strength, G4double brho);
  void BuildQuadrupole(BDSMagnetStrength* strength, G4double brho);
  void BuildSextupole(BDSMagnetStrength*  strength, G4double brho);
  void BuildOctupole(BDSMagnetStrength*   strength, G4double brho);
  void BuildDecapole(BDSMagnetStrength*   strength, G4double brho);
  void BuildMultipole(BDSMagnetStrength*  strength, G4double brho);
  ///@}
  
  /// Splits the G4String member variable formatAndName on the ":" character.
  /// Whatever is before is taken as the fromat string and whatever is after is
  /// taken as the filepath.
  void ParseFormatAndFilename();

  /// Reset all pointers to nullptr that are temporarily used during construction
  /// to avoid mistaken contamination between uses of the factory
  void CleanUp();

  BDSMagFieldMesh* BuildMagField3D();
  BDSMagFieldMesh* BuildMagFieldXY();
  BDSMagFieldMesh* BuildMagFieldLCDD();
  BDSMagFieldMesh* BuildMagFieldSQL();
};
#endif
