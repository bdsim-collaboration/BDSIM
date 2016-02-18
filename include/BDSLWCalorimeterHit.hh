#ifndef BDSLWCALORIMETERHIT_H
#define BDSLWCALORIMETERHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class BDSLWCalorimeterHit :public G4VHit
{
public:
  BDSLWCalorimeterHit(G4int nCopy, G4double Energy=0, G4int nEvent=0);

  virtual ~BDSLWCalorimeterHit();

  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);
   
private:
  G4double itsEnergy;
  G4int itsCopyNumber;
  G4int itsEventNo;

public:

  inline G4double GetEnergy()
    {return itsEnergy;}
  inline G4int GetCopyNumber()
    {return itsCopyNumber;}
  inline void SetEnergy(G4double Energy)
    {itsEnergy=Energy;}   
  inline void AddEnergy(G4double Energy)
    {itsEnergy+=Energy;}
  inline G4int GetEventNo() const
    {return itsEventNo;}
};


typedef G4THitsCollection<BDSLWCalorimeterHit> BDSLWCalorimeterHitsCollection;
extern G4Allocator<BDSLWCalorimeterHit> BDSLWCalorimeterHitAllocator;

inline void* BDSLWCalorimeterHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSLWCalorimeterHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSLWCalorimeterHit::operator delete(void *aHit)
{
 BDSLWCalorimeterHitAllocator.FreeSingle((BDSLWCalorimeterHit*) aHit);
}

#endif

///
