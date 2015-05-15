/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 5.11.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed Samplers to account for plane and cylinder types (GABs code)
*/

#ifndef BDSSamplerHit_h
#define BDSSamplerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "BDSParticle.hh"

class BDSSamplerHit :public G4VHit
{
public:
  BDSSamplerHit(G4String aName,
		BDSParticle init,
		BDSParticle prod,
		BDSParticle last_scat,
		BDSParticle local,
		BDSParticle global,
		G4double s,
		G4double weight,
		G4int PDGtype, G4int nEvent,
		G4int ParentID, G4int TrackID,
		G4int TurnsTaken,
		G4String sampType);

  ~BDSSamplerHit();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);
  
private:
  G4String itsName;

  ///initial particle track in GLOBAL coordinates
  BDSParticle itsInit;

  ///point where the particle was produced
  BDSParticle itsProd;

  ///point where the particle last scattered
  BDSParticle itsLastScat;

  ///actual position and momentum direction in LOCAL coordinates, relative to
  ///the sampler and to the ideal orbit
  BDSParticle itsLocal;

  ///actual position and momentum direction in GLOBAL coordinates
  BDSParticle itsGlobal;

  ///total current track length
  G4double itsS;

  G4double itsWeight;
  G4int itsPDGtype;
  G4int itsEventNo;
  G4int itsParentID;
  G4int itsTrackID;
  G4int itsTurnsTaken;
  G4String itsSampType;
  
public:
  inline G4double GetInitMom() const
    {return itsInit.GetEnergy();}
  inline G4double GetInitX() const
    {return itsInit.GetX();}
  inline G4double GetInitXPrime() const
    {return itsInit.GetXp();}
  inline G4double GetInitY() const
    {return itsInit.GetY();}
  inline G4double GetInitYPrime() const
    {return itsInit.GetYp();}
  inline G4double GetInitZ() const
    {return itsInit.GetZ();}
  inline G4double GetInitZPrime() const
    {return itsInit.GetZp();}
  inline G4double GetInitT() const
    {return itsInit.GetTime();}
  inline G4double GetProdMom() const
    {return itsProd.GetEnergy();}
  inline G4double GetProdX() const
    {return itsProd.GetX();}
  inline G4double GetProdXPrime() const
    {return itsProd.GetXp();}
  inline G4double GetProdY() const
    {return itsProd.GetY();}
  inline G4double GetProdYPrime() const
    {return itsProd.GetYp();}
  inline G4double GetProdZ() const
    {return itsProd.GetZ();}
  inline G4double GetProdZPrime() const
    {return itsProd.GetZp();}
  inline G4double GetProdT() const
    {return itsProd.GetTime();}
  inline G4double GetLastScatMom() const
    {return itsLastScat.GetEnergy();}
  inline G4double GetLastScatX() const
    {return itsLastScat.GetX();}
  inline G4double GetLastScatXPrime() const
    {return itsLastScat.GetXp();}
  inline G4double GetLastScatY() const
    {return itsLastScat.GetY();}
  inline G4double GetLastScatYPrime() const
    {return itsLastScat.GetYp();}
  inline G4double GetLastScatZ() const
    {return itsLastScat.GetZ();}
  inline G4double GetLastScatZPrime() const
    {return itsLastScat.GetZp();}
  inline G4double GetLastScatT() const
    {return itsLastScat.GetTime();}
  inline G4double GetMom() const
    {return itsLocal.GetEnergy();}
  inline G4double GetX() const
    {return itsLocal.GetX();}
  inline G4double GetXPrime() const
    {return itsLocal.GetXp();}
  inline G4double GetY() const
    {return itsLocal.GetY();}
  inline G4double GetYPrime() const
    {return itsLocal.GetYp();}
  inline G4double GetZ() const
    {return itsLocal.GetZ();}
  inline G4double GetZPrime() const
    {return itsLocal.GetZp();}
  inline G4double GetGlobalX() const
    {return itsGlobal.GetX();}
  inline G4double GetGlobalXPrime() const
    {return itsGlobal.GetXp();}
  inline G4double GetGlobalY() const
    {return itsGlobal.GetY();}
  inline G4double GetGlobalYPrime() const
    {return itsGlobal.GetYp();}
  inline G4double GetGlobalZ() const
    {return itsGlobal.GetZ();}
  inline G4double GetGlobalZPrime() const
    {return itsGlobal.GetZp();}
  inline G4double GetT() const
    {return itsLocal.GetTime();}
  inline G4double GetS() const
    {return itsS;}
  inline G4double GetWeight() const
    {return itsWeight;}
  inline G4String GetName() const
    {return itsName;}
  inline G4String GetType()
    {return itsSampType;}
  inline G4int GetEventNo() const
    {return itsEventNo;}
  inline G4int GetPDGtype() const
    {return itsPDGtype;}
  inline G4int GetParentID() const
    {return itsParentID;}
  inline G4int GetTrackID() const
    {return itsTrackID;}
  inline G4int GetTurnsTaken() const
  {return itsTurnsTaken;}
};

typedef G4THitsCollection<BDSSamplerHit> BDSSamplerHitsCollection;
extern G4Allocator<BDSSamplerHit> BDSSamplerHitAllocator;

inline void* BDSSamplerHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSSamplerHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSSamplerHit::operator delete(void *aHit)
{
  BDSSamplerHitAllocator.FreeSingle((BDSSamplerHit*) aHit);
}

#endif

///
