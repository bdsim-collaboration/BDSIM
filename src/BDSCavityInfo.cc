#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"

#include "globals.hh"

BDSCavityInfo::BDSCavityInfo(BDSCavityType cavityTypeIn,
			     G4Material*   materialIn,
			     G4Material*   vacuumMaterialIn,
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
			     G4double      tangentLineAngleIn):
  cavityType(cavityTypeIn),
  material(materialIn),
  vacuumMaterial(vacuumMaterialIn),
  frequency(frequencyIn),
  phase(phaseIn),
  irisRadius(irisRadiusIn),
  thickness(thicknessIn),
  equatorRadius(equatorRadiusIn),
  halfCellLength(halfCellLengthIn),
  numberOfPoints(numberOfPointsIn),
  numberOfCells(numberOfCellsIn),
  equatorEllipseSemiAxis(equatorEllipseSemiAxisIn),
  irisHorizontalAxis(irisHorizontalAxisIn),
  irisVerticalAxis(irisVerticalAxisIn),
  tangentLineAngle(tangentLineAngleIn)
{;}
