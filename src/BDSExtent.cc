#include "BDSExtent.hh"

#include "globals.hh" // geant4 types / globals

#include <algorithm>
#include <utility>
#include <vector>

BDSExtent::BDSExtent():
  extentX(std::make_pair(0.0,0.0)),
  extentY(std::make_pair(0.0,0.0)),
  extentZ(std::make_pair(0.0,0.0))
{;}

BDSExtent::BDSExtent(std::pair<G4double, G4double> extXIn,
		     std::pair<G4double, G4double> extYIn,
		     std::pair<G4double, G4double> extZIn):
  extentX(extXIn),
  extentY(extYIn),
  extentZ(extZIn)
{;}

BDSExtent::BDSExtent(G4double extXNegIn, G4double extXPosIn,
		     G4double extYNegIn, G4double extYPosIn,
		     G4double extZNegIn, G4double extZPosIn):
  extentX(std::make_pair(extXNegIn, extXPosIn)),
  extentY(std::make_pair(extYNegIn, extYPosIn)),
  extentZ(std::make_pair(extZNegIn, extZPosIn))
{;}

BDSExtent::BDSExtent(G4double extXIn, G4double extYIn, G4double extZIn):
  extentX(std::make_pair(-extXIn, extXIn)),
  extentY(std::make_pair(-extYIn, extYIn)),
  extentZ(std::make_pair(-extZIn, extZIn))
{;}

BDSExtent::~BDSExtent()
{;}

BDSExtent BDSExtent::Shift(G4double x, G4double y) const
{
  BDSExtent xShifted = ShiftX(x);
  return xShifted.ShiftY(y);
}

BDSExtent BDSExtent::ShiftX(G4double x) const
{
  BDSExtent result = BDSExtent(extentX.first + x, extentX.second + x,
			       extentY.first, extentY.second,
			       extentZ.first, extentZ.second);
  return result;
}

BDSExtent BDSExtent::ShiftY(G4double y) const
{
  BDSExtent result = BDSExtent(extentX.first, extentX.second,
			       extentY.first + y, extentY.second + y,
			       extentZ.first, extentZ.second);
  return result;
}

G4double BDSExtent::MaximumAbs() const
{
  std::vector<G4double> exts = {std::abs(extentX.first), extentX.second,
				std::abs(extentY.first), extentY.second,
				std::abs(extentZ.first), extentZ.second};
  return *std::max_element(exts.begin(), exts.end());
}
