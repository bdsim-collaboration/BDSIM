/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSEnergyCounterHit_h
#define BDSEnergyCounterHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class BDSEnergyCounterHit :public G4VHit
{
public:
  BDSEnergyCounterHit(G4int    nCopy, 
		      G4double Energy=0, 
		      G4double x=0, 
		      G4double y=0, 
		      G4double z=0,
		      G4double s=0,
		      G4String name="", 
		      G4int    partID=0, 
		      G4double weight=1, 
		      G4bool   precisionRegion=false, 
		      G4int    TurnsTaken=1);

  ~BDSEnergyCounterHit();

  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  inline G4double GetEnergy();
  inline G4int    GetCopyNumber();
  inline G4String GetName();
  inline void     SetEnergy(G4double Energy);
  inline void     AddEnergy(G4double Energy, G4double weight);
  inline void     AddEnergyWeightedPosition(G4double Energy, 
					    G4double xpos, 
					    G4double ypos, 
					    G4double zpos, 
					    G4double S,
					    G4double weight);
  inline G4double GetZ(); 
  inline G4double GetX(); 
  inline G4double GetY();
  inline G4double GetS();
  inline G4double GetEnergyWeightedY(); 
  inline G4double GetEnergyWeightedZ(); 
  inline G4double GetEnergyWeightedX();
  inline G4double GetEnergyWeightedS();
  inline G4int    GetPartID(); 
  inline G4double GetWeight(); 
  inline void     SetWeight(G4double weight);
  inline G4bool   GetPrecisionRegion();
  inline G4int    GetTurnsTaken();
  inline void     SetTurnsTaken(G4int turnstaken);
  
private:
  /// private default constructor (not implemented)
  BDSEnergyCounterHit();

  G4double itsEnergy;
  G4double itsEnergyWeightedX;
  G4double itsEnergyWeightedY;
  G4double itsEnergyWeightedZ;
  G4double itsEnergyWeightedS;
  G4double itsX;
  G4double itsY;
  G4double itsZ;
  G4double itsS;
  G4double itsWeight;
  G4int    itsCopyNumber;
  G4int    itsPartID;
  G4String itsName;
  G4bool   itsPrecisionRegion; //Whether or not the hit is in the precision region
  G4int    itsTurnsTaken;
};

inline G4double BDSEnergyCounterHit::GetEnergy()
{return itsEnergy;}

inline G4double BDSEnergyCounterHit::GetX()
{return itsX;}

inline G4double BDSEnergyCounterHit::GetY()
{return itsY;}

inline G4double BDSEnergyCounterHit::GetZ()
{return itsZ;}

inline G4double BDSEnergyCounterHit::GetS()
{return itsS;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedX()
{return itsEnergyWeightedX;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedY()
{return itsEnergyWeightedY;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedZ()
{return itsEnergyWeightedZ;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedS()
{return itsEnergyWeightedS;}

inline G4double BDSEnergyCounterHit::GetWeight()
{return itsWeight;}

inline G4bool BDSEnergyCounterHit::GetPrecisionRegion()
{return itsPrecisionRegion;}

inline G4int BDSEnergyCounterHit::GetCopyNumber()
{return itsCopyNumber;}

inline G4int BDSEnergyCounterHit::GetPartID()
{return itsPartID;}

inline G4String BDSEnergyCounterHit::GetName()
{return itsName;}

inline void BDSEnergyCounterHit::SetEnergy(G4double Energy)
{itsEnergy=Energy;}   

inline void BDSEnergyCounterHit::AddEnergy(G4double Energy, G4double weight)
{itsEnergy+=Energy*weight;}   

inline void BDSEnergyCounterHit::AddEnergyWeightedPosition(G4double Energy, G4double X, G4double Y, G4double Z, G4double S, G4double weight){
  itsEnergyWeightedX+=Energy*X*weight;
  itsEnergyWeightedY+=Energy*Y*weight;
  itsEnergyWeightedZ+=Energy*Z*weight;
  itsEnergyWeightedS+=Energy*S*weight;
  itsEnergy+=Energy*weight;
}

typedef G4THitsCollection<BDSEnergyCounterHit> BDSEnergyCounterHitsCollection;
extern G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

inline void* BDSEnergyCounterHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSEnergyCounterHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSEnergyCounterHit::operator delete(void *aHit)
{
 BDSEnergyCounterHitAllocator.FreeSingle((BDSEnergyCounterHit*) aHit);
}

inline G4int BDSEnergyCounterHit::GetTurnsTaken()
{return itsTurnsTaken;}

inline void  BDSEnergyCounterHit::SetTurnsTaken(G4int turnstaken)
{itsTurnsTaken = turnstaken;}

#endif

///
