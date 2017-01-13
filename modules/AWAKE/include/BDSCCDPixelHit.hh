#ifndef BDSCCDPIXELHIT_H
#define BDSCCDPIXELHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class BDSCCDPixelHit :public G4VHit
{
public:
  BDSCCDPixelHit(G4int npixel,
		 G4double weight);

  ~BDSCCDPixelHit();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);
  
private:
  G4int _npixel;
  G4double _weight;
  
public:
  inline G4int npixel() const
  {return _npixel;}
  inline G4double weight() const
  {return _weight;}
};

typedef G4THitsCollection<BDSCCDPixelHit> BDSCCDPixelHitsCollection;
extern G4Allocator<BDSCCDPixelHit> BDSCCDPixelHitAllocator;

inline void* BDSCCDPixelHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSCCDPixelHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSCCDPixelHit::operator delete(void *aHit)
{
  BDSCCDPixelHitAllocator.FreeSingle((BDSCCDPixelHit*) aHit);
}

#endif

///
