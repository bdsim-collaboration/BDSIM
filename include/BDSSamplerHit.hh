/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BDSSAMPLERHIT_H
#define BDSSAMPLERHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

#include "BDSParticleCoordsFull.hh"

#include <ostream>

/**
 * @brief The information recorded from a particle impacting a sampler.
 *
 * Everything public for simplicity of the class.
 */

class BDSSamplerHit: public G4VHit
{
public:
  BDSSamplerHit(G4int samplerIDIn,
		const BDSParticleCoordsFull& coordsIn,
		G4int pdgIDIn,
		G4int parentIDIn,
		G4int trackIDIn,
		G4int turnsTakenIn,
		G4int beamlineIndexIn);
  
  virtual ~BDSSamplerHit(){;}
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  G4int                 samplerID;
  BDSParticleCoordsFull coords;
  G4int                 pdgID;
  G4int                 parentID;
  G4int                 trackID;
  G4int                 turnsTaken;
  G4int                 beamlineIndex;
  
<<<<<<< HEAD
public:
  inline G4int  GetSamplerID()             const {return itsSamplerID;}
  inline G4double GetInitTotalEnergy()     const {return itsInit.GetTotalEnergy();}
  inline BDSParticle GetInit()             const {return itsInit;}
  inline G4double GetInitX()               const {return itsInit.GetX();}
  inline G4double GetInitXPrime()          const {return itsInit.GetXp();}
  inline G4double GetInitY()               const {return itsInit.GetY();}
  inline G4double GetInitYPrime()          const {return itsInit.GetYp();}
  inline G4double GetInitZ()               const {return itsInit.GetZ();}
  inline G4double GetInitZPrime()          const {return itsInit.GetZp();}
  inline G4double GetInitT()               const {return itsInit.GetT();}
  inline BDSParticle GetProd()             const {return itsProd;}
  inline G4double GetProdTotalEnergy()     const {return itsProd.GetTotalEnergy();}
  inline G4double GetProdX()               const {return itsProd.GetX();}
  inline G4double GetProdXPrime()          const {return itsProd.GetXp();}
  inline G4double GetProdY()               const {return itsProd.GetY();}
  inline G4double GetProdYPrime()          const {return itsProd.GetYp();}
  inline G4double GetProdZ()               const {return itsProd.GetZ();}
  inline G4double GetProdZPrime()          const {return itsProd.GetZp();}
  inline G4double GetProdT()               const {return itsProd.GetT();}
  inline BDSParticle GetLastScat()         const {return itsLastScat;}
  inline G4double GetLastScatTotalEnergy() const {return itsLastScat.GetTotalEnergy();}
  inline G4double GetLastScatX()           const {return itsLastScat.GetX();}
  inline G4double GetLastScatXPrime()      const {return itsLastScat.GetXp();}
  inline G4double GetLastScatY()           const {return itsLastScat.GetY();}
  inline G4double GetLastScatYPrime()      const {return itsLastScat.GetYp();}
  inline G4double GetLastScatZ()           const {return itsLastScat.GetZ();}
  inline G4double GetLastScatZPrime()      const {return itsLastScat.GetZp();}
  inline G4double GetLastScatT()    const {return itsLastScat.GetT();}
  inline BDSParticle GetLocal()     const {return itsLocal;}
  inline G4double GetTotalEnergy()  const {return itsLocal.GetTotalEnergy();}
  inline G4double GetX()            const {return itsLocal.GetX();}
  inline G4double GetXPrime()       const {return itsLocal.GetXp();}
  inline G4double GetY()            const {return itsLocal.GetY();}
  inline G4double GetYPrime()       const {return itsLocal.GetYp();}
  inline G4double GetZ()            const {return itsLocal.GetZ();}
  inline G4double GetZPrime()       const {return itsLocal.GetZp();}
  inline BDSParticle GetGlobal()    const {return itsGlobal;}
  inline G4double GetGlobalX()      const {return itsGlobal.GetX();}
  inline G4double GetGlobalXPrime() const {return itsGlobal.GetXp();}
  inline G4double GetGlobalY()      const {return itsGlobal.GetY();}
  inline G4double GetGlobalYPrime() const {return itsGlobal.GetYp();}
  inline G4double GetGlobalZ()      const {return itsGlobal.GetZ();}
  inline G4double GetGlobalZPrime() const {return itsGlobal.GetZp();}
  inline G4double GetT()            const {return itsLocal.GetT();}
  inline G4double GetS()            const {return itsS;}
  inline G4double GetWeight()       const {return itsWeight;}
  inline G4String GetName()         const {return itsName;}
  inline G4int GetEventNo()         const {return itsEventNo;}
  inline G4int GetPDGtype()         const {return itsPDGtype;}
  inline G4int GetParentID()        const {return itsParentID;}
  inline G4int GetTrackID()         const {return itsTrackID;}
  inline G4int GetTurnsTaken()      const {return itsTurnsTaken;}
  inline G4String GetProcess()      const {return itsProcess;}
  inline G4int GetBeamlineIndex()   const {return itsBeamlineIndex;}
=======
private:
  BDSSamplerHit() = delete; ///< No default constructor.
>>>>>>> develop
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
