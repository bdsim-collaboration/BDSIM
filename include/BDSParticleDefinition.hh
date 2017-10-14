#ifndef BDSPARTICLEDEFINITION_H
#define BDSPARTICLEDEFINITION_H

#include "globals.hh"

#include <ostream>

class G4ParticleDefinition;

class BDSIonDefinition;

/**
 * @brief Wrapper for particle definition.
 *
 * Used to calculate momentum and kinetic energy and associate any other information
 * required along with a G4ParticleDefinition instance.
 *
 * @author Laurie Nevay
 */

class BDSParticleDefinition
{
public:
  /// Constructor requires G4 particle definition as well as user specified
  /// total energy. From this the momentum and kinetic energy are calculated
  /// and checked to be valid - ie sufficient total energy. The optional ion
  /// definition may be supplied to override the charge of the ion.
  BDSParticleDefinition(G4ParticleDefinition* particleIn,
			G4double              totalEnergyIn,
			BDSIonDefinition*     ionDefinitionIn = nullptr);
  ~BDSParticleDefinition();

  /// @{ Accessor.
  inline G4ParticleDefinition* ParticleDefinition() const {return particle;}
  inline G4double Charge()        const {return charge;}
  inline G4double TotalEnergy()   const {return totalEnergy;}
  inline G4double KineticEnergy() const {return kineticEnergy;}
  inline G4double Momentum()      const {return momentum;}
  inline G4double BRho()          const {return brho;}
  /// @}

  /// Output stream operator implementation.
  friend std::ostream& operator<< (std::ostream& out, BDSParticleDefinition const& def);
  
private:
  /// No default constructor.
  BDSParticleDefinition() = delete;
  
  G4ParticleDefinition* particle;      ///< Does not own.
  BDSIonDefinition*     ionDefinition; ///< Optional ion definition. Does own.

  G4double charge;         ///< Particle charge.
  G4double totalEnergy;    ///< Particle total energy.
  G4double kineticEnergy;  ///< Particle kinetic energy.
  G4double momentum;       ///< Particle momentum.
  G4double brho;           ///< Particle rigidity.
};

#endif
