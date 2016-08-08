#include "BDSExtent.hh"

#include "globals.hh" // geant4 types / globals

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
