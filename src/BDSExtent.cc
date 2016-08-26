#include "BDSExtent.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4TwoVector.hh"

#include <algorithm>
#include <ostream>
#include <utility>

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

BDSExtent BDSExtent::Tilted(G4double angle) const
{
  if (!BDS::IsFinite(angle))
    {return BDSExtent(*this);}

  // rotate each vector (from origin to each corner) by angle
  // and check - safer than checking based on +ve / -ve angle
  G4TwoVector topRight = G4TwoVector(extentX.second, extentY.second);
  G4TwoVector botRight = G4TwoVector(extentX.second, extentY.first);
  G4TwoVector botLeft  = G4TwoVector(extentX.first,  extentY.first);
  G4TwoVector topLeft  = G4TwoVector(extentX.first,  extentY.second);

  G4double xMin = std::min(botLeft.x(),  topLeft.x());
  G4double xMax = std::max(topRight.x(), botRight.x());
  G4double yMin = std::min(botRight.y(), botLeft.y());
  G4double yMax = std::max(topRight.y(), topLeft.y());

  BDSExtent result = BDSExtent(xMin, xMax,
			       yMin, yMax,
			       extentZ.first, extentZ.second);
  return result;
}

std::ostream& operator<< (std::ostream& out, BDSExtent const& ext)
{
  out << ext.extentX.first << " " << ext.extentX.second << " ";
  out << ext.extentY.first << " " << ext.extentY.second << " ";
  out << ext.extentZ.first << " " << ext.extentZ.second;
  return out;
}
