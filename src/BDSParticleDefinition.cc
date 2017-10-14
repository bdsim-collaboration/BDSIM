#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "G4ParticleDefinition.hh"

#include <cmath>
#include <ostream>
#include <stdexcept>


BDSParticleDefinition::BDSParticleDefinition(G4ParticleDefinition* particleIn,
					     G4double              totalEnergyIn,
					     BDSIonDefinition*     ionDefinitionIn):
  particle(particleIn),
  ionDefinition(ionDefinitionIn),
  totalEnergy(totalEnergyIn),
  brho(DBL_MAX)// if zero charge infinite magnetic rigidity
{
  // charge
  charge = particle->GetPDGCharge();
  if (ionDefinition) // may be nullptr
    {
      if (ionDefinition->OverrideCharge()) // if override for ions
	{charge = ionDefinition->Charge();}
    }

  // kinetic energy
  G4double mass = particle->GetPDGMass();
  kineticEnergy = totalEnergy - mass;

  // momentum
  try
    {momentum = std::sqrt(std::pow(totalEnergy,2) - std::pow(mass,2));}
  catch (std::domain_error) // sqrt(-ve)
    {
      G4cerr << __METHOD_NAME__ << "Total energy insufficient to include mass or particle" << G4endl;
      exit(1);
    }

  // magnetic rigidity (brho)
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * charge(e))
  // charge (in e units); rigidity (in T*m)
  if (BDS::IsFinite(charge))
    {
      BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
      brho = globals->FFact() * momentum / CLHEP::GeV / globals->COverGeV() / charge;
      brho *= CLHEP::tesla*CLHEP::m; // rigidity (in Geant4 units)
    }
}

BDSParticleDefinition::~BDSParticleDefinition()
{
  delete ionDefinition;
}

std::ostream& operator<<(std::ostream& out, const BDSParticleDefinition& def)
{
  out << "Particle:        " << def.particle->GetParticleName() << G4endl;
  out << "Mass:            " << def.particle->GetPDGMass()/CLHEP::GeV << " GeV" << G4endl;
  out << "Charge:          " << def.charge                       << " e" << G4endl;
  out << "Total Energy:    " << def.totalEnergy/CLHEP::GeV       << " GeV" << G4endl;
  out << "Kinetic Energy:  " << def.kineticEnergy/CLHEP::GeV     << " GeV" << G4endl;
  out << "Momentum:        " << def.momentum/CLHEP::GeV          << " GeV" << G4endl;
  out << "Rigidity (Brho): " << def.brho/(CLHEP::tesla*CLHEP::m) << " T*m"<<G4endl;
  return out;
}


