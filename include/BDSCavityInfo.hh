#ifndef BDSCAVITYINFO_H
#define BDSCAVITYINFO_H

#include "BDSCavityType.hh"
#include "globals.hh"         // geant4 types / globals

#include "CLHEP/Units/SystemOfUnits.h"

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
 * This class does not specify the vacuum material as ideally we can specify the
 * the 'vacuum' pressure and composition throughout the machine without redefining
 * a cavity model.
 * 
 * @author Laurie Nevay
 */

class BDSCavityInfo
{
public:
  /// Constructor to assign all members at once. Default values are based
  /// on those for SRF cavity.
  BDSCavityInfo(BDSCavityType cavityTypeIn,
		G4Material*   materialIn,
		G4double      irisRadiusIn,
		G4double      thicknessIn,
		G4double      equatorRadiusIn,
		G4double      halfCellLengthIn,
		G4int         numberOfPointsIn         = 24,
		G4int         numberOfCellsIn          = 1,
		G4double      equatorHorizontalAxisIn  = 42*CLHEP::mm,
		G4double      equatorVerticalAxisIn    = 42*CLHEP::mm,
		G4double      irisHorizontalAxisIn     = 12*CLHEP::mm,
		G4double      irisVerticalAxisIn       = 19*CLHEP::mm,
		G4double      tangentLineAngleIn       = 13.3*CLHEP::degree);

  /// Cavity type.
  BDSCavityType cavityType;

  /// Material.
  G4Material* material;

  /// Iris radius - half width of narrowest part.
  G4double irisRadius;

  /// Thickness of wall material.
  G4double thickness;

  /// Equator radius - half width of widest part.
  G4double equatorRadius;

  /// Half cell length.
  G4double halfCellLength;

  /// Number of points that will be used to make the cell shape
  /// more will be more accurate but will also increase tracking time.
  G4int numberOfPoints;

  /// Number of cells per cavity.
  G4int numberOfCells;

  /// Equator ellipse horizontal semi-axis.
  G4double equatorHorizontalAxis;

  /// Equator ellipse vertical semi-axis.
  G4double equatorVerticalAxis;

  /// Iris ellipse horizontal semi-axis.
  G4double irisHorizontalAxis;

  /// Iris ellipse vertical semi-axis.
  G4double irisVerticalAxis;

  /// Tangent angle.
  G4double tangentLineAngle;

private:
  /// private default constructor as unused
  BDSCavityInfo();
};

#endif
