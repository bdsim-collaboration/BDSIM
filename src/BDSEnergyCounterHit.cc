#include "BDSEnergyCounterHit.hh"
#include "G4ios.hh"

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::BDSEnergyCounterHit(G4int    nCopyIn, 
					 G4double energyIn, 
					 G4double XIn, 
					 G4double YIn, 
					 G4double ZIn,
					 G4double SIn,
					 G4double xIn,
					 G4double yIn,
					 G4double zIn,
					 G4String nameIn, 
					 G4int    partIDIn, 
					 G4double weightIn, 
					 G4bool   precisionRegionIn, 
					 G4int    turnsTakenIn,
					 G4int    eventNoIn):
  copyNumber(nCopyIn),
  energy(energyIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  S(SIn),
  x(xIn),
  y(yIn),
  z(zIn),
  name(nameIn),
  partID(partIDIn),
  weight(weightIn),
  precisionRegion(precisionRegionIn),
  turnsTaken(turnsTakenIn),
  eventNo(eventNoIn)
{;}

BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}

BDSEnergyCounterHit* BDS::LowestSPosPrimaryHit(BDSEnergyCounterHitsCollection* hc)
{
  G4double sposMin = 1e20; //stupidly large number - spos will always be less than
  G4double spos = 0;
  G4int indexofHit = -1;
  for (G4int i = 0; i < hc->entries(); ++i)
    {
      spos = (*hc)[i]->GetS();
      if (spos < sposMin) {
	sposMin = spos; //keep for testing
	indexofHit = i; //record which hit it was
      }
    }
  if (indexofHit != -1)
    {return (*hc)[indexofHit];}
  else
    {return nullptr;}
}


BDSEnergyCounterHit* BDS::HighestSPosPrimaryHit(BDSEnergyCounterHitsCollection* hc)
{
  G4double sposMax = -1e10; //negative number - spos will always be greater than
  G4double spos = 0;
  G4int indexofHit = -1;
  for (G4int i = 0; i < hc->entries(); ++i)
    {
      spos = (*hc)[i]->GetS();
      if (spos > sposMax) {
	sposMax = spos; //keep for testing
	indexofHit = i; //record which hit it was
      }
    }
  if (indexofHit != -1)
    {return (*hc)[indexofHit];}
  else
    {return nullptr;}
}
