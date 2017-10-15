#ifndef BDSIONDEFINITION_H
#define BDSIONDEFINITION_H

#include "globals.hh"

#include <ostream>

/**
 * @brief Class to parse an ion particle definition.
 *
 * @author Laurie Nevay
 */

class BDSIonDefinition
{
public:
  BDSIonDefinition(G4String definition);
  ~BDSIonDefinition(){;}

  /// @{ Accessor.
  inline G4int    A()                const {return a;}
  inline G4int    Z()                const {return z;}
  inline G4double Charge()           const {return charge;}
  inline G4double ExcitationEnergy() const {return energy;}
  inline G4bool   OverrideCharge()   const {return overrideCharge;}
  /// @}

  /// Output streamer.
  friend std::ostream& operator<< (std::ostream& out, BDSIonDefinition const& io);

private:
  /// No default constructor.
  BDSIonDefinition() = delete;

  /// Parse the definition.
  void Parse(const G4String& definition);

  G4int    a;
  G4int    z;
  G4double charge;         ///< In units of eplus
  G4double energy;         ///< Excitation energy. G4units.
  G4bool   overrideCharge; ///< Whether to specify the charge.
};

#endif
