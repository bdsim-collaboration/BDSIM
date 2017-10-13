#ifndef BDSIONDEFINITION_H
#define BDSIONDEFINITION_H

#include "globals.hh"

#include <ostream>

class BDSIonDefinition
{
public:
  BDSIonDefinition(G4String definition);
  ~BDSIonDefinition(){;}

  inline G4int A() const {return a;}
  inline G4int Z() const {return z;}
  inline G4int Charge() const {return charge;}
  inline G4double ExcitationEnergy() const {return energy;}

  friend std::ostream& operator<< (std::ostream& out, BDSIonDefinition const& io);

private:
  BDSIonDefinition() = delete;

  /// Parse the definition.
  void Parse(const G4String& definition);

  G4int a;
  G4int z;
  G4int charge;    ///< In units of eplus
  G4double energy; ///< Excitation energy. G4units.
};

#endif
