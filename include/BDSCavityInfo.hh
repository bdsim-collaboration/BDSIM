#ifndef BDSCAVITYINFO_H
#define BDSCAVITYINFO_H

#include "BDSCavityType.hh"
#include "globals.hh"         // geant4 types / globals

class G4Material;

/**
 * @brief Holder for all Geometrical information required to create an RF cavity. 
 * 
 * Geant4 units are assumed by classes that use instances of this class.
 * This reduces the number of argument to constructors 
 * plus aggregates common tasks in the component factory.
 *
 * Note, compiler provided copy constructor is sufficient as only pointers
 * to materials owned by BDSMaterials are kept - shallow copy is required then.
 * 
 * @author Laurie Nevay
 */

class BDSCavityInfo
{
public:
  /// extra constructor to assign all members at once
  BDSCavityInfo(BDSCavityType cavityTypeIn,
		G4Material*   materialIn,
		G4Material*   vacuumMaterialIn,
		G4double      eFieldIn,
		G4double      frequencyIn,
		G4double      phaseIn,
		G4double      irisRadiusIn,
		G4double      thicknessIn,
		G4double      equatorRadiusIn,
		G4double      halfCellLengthIn,
		G4int         numberOfPointsIn,
		G4int         numberOfCellsIn,
		G4double      equatorEllipseSemiAxisIn,
		G4double      irisHorizontalAxisIn,
		G4double      irisVerticalAxisIn,
		G4double      tangentLineAngleIn);

  /// Cavity type
  BDSCavityType cavityType;

  /// Material
  G4Material* material;

  /// Vacuum Material
  G4Material* vacuumMaterial;

  /// Peak Electric Field in V / m
  G4double eField;

  /// Frequency in MHz
  G4double frequency;

  /// Phase (rad)
  G4double phase;

  /// Iris radius
  G4double irisRadius;

  /// Thickness
  G4double thickness;

  /// Equator radius
  G4double equatorRadius;

  /// Half cell length
  G4double halfCellLength;

  /// number of points that will be used to make the cell shape
  /// more will be more accurate but will also increase tracking time
  G4int numberOfPoints;

  /// number of cells per cavity
  G4int numberOfCells;

  /// Equator ellipse semi axis;
  G4double equatorEllipseSemiAxis;

  /// Iris horizontal axis
  G4double irisHorizontalAxis;

  /// Iris vertical axis
  G4double irisVerticalAxis;

  /// Tangent angle
  G4double tangentLineAngle;

private:
  /// private default constructor as unused
  BDSCavityInfo();
};

#endif
