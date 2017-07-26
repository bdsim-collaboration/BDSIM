#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"

#include "globals.hh"

BDSCavityInfo::BDSCavityInfo(BDSCavityType cavityTypeIn,
			     G4Material*   materialIn,
			     G4double      irisRadiusIn,
			     G4double      thicknessIn,
			     G4double      equatorRadiusIn,
			     G4double      halfCellLengthIn,
			     G4int         numberOfPointsIn,
			     G4int         numberOfCellsIn,
			     G4double      equatorHorizontalAxisIn,
			     G4double      equatorVerticalAxisIn,
			     G4double      irisHorizontalAxisIn,
			     G4double      irisVerticalAxisIn,
			     G4double      tangentLineAngleIn):
  cavityType(cavityTypeIn),
  material(materialIn),
  irisRadius(irisRadiusIn),
  thickness(thicknessIn),
  equatorRadius(equatorRadiusIn),
  halfCellLength(halfCellLengthIn),
  numberOfPoints(numberOfPointsIn),
  numberOfCells(numberOfCellsIn),
  equatorHorizontalAxis(equatorHorizontalAxisIn),
  equatorVerticalAxis(equatorVerticalAxisIn),
  irisHorizontalAxis(irisHorizontalAxisIn),
  irisVerticalAxis(irisVerticalAxisIn),
  tangentLineAngle(tangentLineAngleIn)
{;}
