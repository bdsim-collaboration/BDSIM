/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "BDSSamplerHit.hh"
#include "G4ios.hh"

G4Allocator<BDSSamplerHit> BDSSamplerHitAllocator;

BDSSamplerHit::BDSSamplerHit(G4String    aName,
			     G4int       samplerID,
			     BDSParticle init,
			     BDSParticle prod,
			     BDSParticle last_scat,
			     BDSParticle local,
			     BDSParticle global,
			     G4double    s,
			     G4double    weight,
			     G4int       PDGtype,
			     G4int       nEvent,
			     G4int       ParentID,
			     G4int       TrackID,
			     G4int       TurnsTaken,
			     G4String    process,
			     G4int       beamlineIndex):
  itsName(aName),
  itsSamplerID(samplerID),
  itsInit(init),
  itsProd(prod),
  itsLastScat(last_scat),
  itsLocal(local),
  itsGlobal(global),
  itsS(s),
  itsWeight(weight),
  itsPDGtype(PDGtype),
  itsEventNo(nEvent),
  itsParentID(ParentID),
  itsTrackID(TrackID),
  itsTurnsTaken(TurnsTaken),
  itsProcess(process),
  itsBeamlineIndex(beamlineIndex)
{;}

BDSSamplerHit::~BDSSamplerHit()
{;}

std::ostream& operator<< (std::ostream& out, BDSSamplerHit const& h)
{
  out << "Sampler: \"" << h.GetName() << "\", ID: " << h.GetSamplerID() << G4endl;
  out << "Local point:  " << h.GetLocal();
  out << "Global point: " << h.GetGlobal();
  out << "S, weight, PDGID, Event#, ParentID, TrackID, Turns:" << G4endl;
  out << h.GetS()          << ", "
      << h.GetWeight()     << ", "
      << h.GetPDGtype()    << ", "
      << h.GetEventNo()    << ", "
      << h.GetParentID()   << ", "
      << h.GetTrackID()    << ", "
      << h.GetTurnsTaken() << G4endl;
  return out;
}
