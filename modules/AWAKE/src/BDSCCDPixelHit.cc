#include "BDSCCDPixelHit.hh"
#include "G4ios.hh"

G4Allocator<BDSCCDPixelHit> BDSCCDPixelHitAllocator;

BDSCCDPixelHit::BDSCCDPixelHit(G4int npixel,
			     G4double weight)
  : _npixel(npixel), _weight(weight)
  {;}

BDSCCDPixelHit::~BDSCCDPixelHit()
{;}
