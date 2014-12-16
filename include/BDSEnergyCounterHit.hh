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

//LowestSPosPrimaryHit declaration at the bottom of this file
//HighestSPosPrimaryHit declaration at the bottom of this file

class BDSEnergyCounterHit :public G4VHit
{
public:
  BDSEnergyCounterHit(G4int    nCopyIn = 0, 
		      G4double energyIn = 0, 
		      G4double XIn = 0, 
		      G4double YIn = 0, 
		      G4double ZIn = 0,
		      G4double SIn = 0,
		      G4double xIn = 0,
		      G4double yIn = 0,
		      G4double zIn = 0,
		      G4String nameIn = "", 
		      G4int    partIDIn = 0, 
		      G4double weightIn = 1, 
		      G4bool   precisionRegionIn = false, 
		      G4int    turnsTakenIn = 1,
		      G4int    eventNoIn = 0
		      );

  ~BDSEnergyCounterHit();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  inline G4double GetEnergy();
  inline G4int    GetCopyNumber();
  inline G4String GetName();
  inline void     SetEnergy(G4double energyIn);
  inline void     AddEnergy(G4double EnergyIn, G4double weightIn);
  inline void     AddEnergyWeightedPosition(G4double energyIn, 
					    G4double XIn, 
					    G4double YIn,
					    G4double ZIn, 
					    G4double SIn,
					    G4double weightIn);
  inline G4double GetX(); 
  inline G4double GetY();
  inline G4double GetZ(); 
  inline G4double GetS();
  inline G4double Getx(); 
  inline G4double Gety();
  inline G4double Getz(); 
  inline G4double GetEnergyWeightedY(); 
  inline G4double GetEnergyWeightedZ(); 
  inline G4double GetEnergyWeightedX();
  inline G4double GetEnergyWeightedS();
  inline G4int    GetPartID();
  inline void     SetPartID(G4int partIDIn);
  inline G4double GetWeight(); 
  inline void     SetWeight(G4double weightIn);
  inline G4bool   GetPrecisionRegion();
  inline void     SetPrecisionRegion(G4bool precisionRegionIn);
  inline G4int    GetTurnsTaken();
  inline void     SetTurnsTaken(G4int turnstakenIn);
  inline G4int    GetEventNo();
  inline void     SetEventNo(G4int eventNoIn);
  
private:
  /// private default constructor (not implemented)
  BDSEnergyCounterHit();

  G4double energy;
  G4double energyWeightedX;
  G4double energyWeightedY;
  G4double energyWeightedZ;
  G4double energyWeightedS;
  // global coordinates
  G4double X;
  G4double Y;
  G4double Z;
  G4double S;
  // local coordinates
  G4double x;
  G4double y;
  G4double z;
  G4double weight;
  G4int    copyNumber;
  G4int    partID;
  G4String name;
  G4bool   precisionRegion; //Whether or not the hit is in the precision region
  G4int    turnsTaken;
  G4int    eventNo;
};

inline G4double BDSEnergyCounterHit::GetEnergy()
{return energy;}

inline G4double BDSEnergyCounterHit::GetX()
{return X;}

inline G4double BDSEnergyCounterHit::GetY()
{return Y;}

inline G4double BDSEnergyCounterHit::GetZ()
{return Z;}

inline G4double BDSEnergyCounterHit::GetS()
{return S;}

inline G4double BDSEnergyCounterHit::Getx()
{return x;}

inline G4double BDSEnergyCounterHit::Gety()
{return y;}

inline G4double BDSEnergyCounterHit::Getz()
{return z;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedX()
{return energyWeightedX;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedY()
{return energyWeightedY;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedZ()
{return energyWeightedZ;}

inline G4double BDSEnergyCounterHit::GetEnergyWeightedS()
{return energyWeightedS;}

inline G4double BDSEnergyCounterHit::GetWeight()
{return weight;}

inline G4int BDSEnergyCounterHit::GetCopyNumber()
{return copyNumber;}

inline G4int BDSEnergyCounterHit::GetPartID()
{return partID;}

inline G4String BDSEnergyCounterHit::GetName()
{return name;}

inline void BDSEnergyCounterHit::SetEnergy(G4double energyIn)
{energy = energyIn;}   

inline void BDSEnergyCounterHit::AddEnergy(G4double energyIn, G4double weightIn)
{energy += energyIn * weightIn;}   

inline void BDSEnergyCounterHit::AddEnergyWeightedPosition(G4double energyIn,
							   G4double XIn,
							   G4double YIn,
							   G4double ZIn,
							   G4double SIn,
							   G4double weightIn)
{
  energyWeightedX += energyIn * XIn * weightIn;
  energyWeightedY += energyIn * YIn * weightIn;
  energyWeightedZ += energyIn * ZIn * weightIn;
  energyWeightedS += energyIn * SIn * weightIn;
  energy          += energyIn * weightIn;
}

inline G4bool BDSEnergyCounterHit::GetPrecisionRegion()
{return precisionRegion;}

inline void BDSEnergyCounterHit::SetPrecisionRegion(G4bool precisionRegionIn)
{precisionRegion = precisionRegionIn;}

inline G4int BDSEnergyCounterHit::GetTurnsTaken()
{return turnsTaken;}

inline void  BDSEnergyCounterHit::SetTurnsTaken(G4int turnstakenIn)
{turnsTaken = turnstakenIn;}

inline G4int BDSEnergyCounterHit::GetEventNo()
{return eventNo;}

inline void BDSEnergyCounterHit::SetEventNo(G4int eventNoIn)
{eventNo = eventNoIn;}

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


namespace BDS {
  BDSEnergyCounterHit* LowestSPosPrimaryHit (BDSEnergyCounterHitsCollection* HC);
  BDSEnergyCounterHit* HighestSPosPrimaryHit(BDSEnergyCounterHitsCollection* HC);
}

#endif

///
