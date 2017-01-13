#include "BDSParticle.hh"

#include <ostream>

BDSParticle::BDSParticle():
  position(0,0,0),
  momentum(0,0,0),
  totalEnergy(0.0),
  t(0.0),
  weight(1.0),
  trackID(-1),
  parentID(-1)
{;}

BDSParticle::BDSParticle(G4double x,
			 G4double y,
			 G4double z,
			 G4double xp,
			 G4double yp,
			 G4double zp,
			 G4double totalEnergyIn,
			 G4double tIn,
			 G4double weightIn,
			 G4int trackIDIn,
			 G4int parentIDIn):
  position(x,y,z),
  momentum(xp,yp,zp),
  totalEnergy(totalEnergyIn),
  t(tIn),
  weight(weightIn),
  trackID(trackIDIn),
  parentID(parentIDIn)
{;}

BDSParticle::BDSParticle(G4ThreeVector pos,
			 G4ThreeVector mom,
			 G4double      totalEnergyIn,
			 G4double      tIn,
			 G4double      weightIn,
			 G4int         trackIDIn,
			 G4int         parentIDIn):
  position(pos),
  momentum(mom),
  totalEnergy(totalEnergyIn),
  t(tIn),
  weight(weightIn),
  trackID(trackIDIn),
  parentID(parentIDIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSParticle const& p)
{
  out << "Total E: " << p.GetTotalEnergy() << " MeV" << G4endl;
  out << "Position: (" << p.GetX() << ", " << p.GetY() << ", " << p.GetZ() << ")" << G4endl;
  out << "Momentum: (" << p.GetXp() << ", " << p.GetYp() << ", " << p.GetZp() << ")" << G4endl;
  return out;
}
