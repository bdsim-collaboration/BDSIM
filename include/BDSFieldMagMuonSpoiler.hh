#ifndef BDSFIELDMAGMUONSPOILER_H
#define BDSFIELDMAGMUONSPOILER_H

#include "BDSField.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

/** 
 * @brief Field of a Muon Spoiler.
 *
 */

class BDSFieldMagMuonSpoiler: public BDSField
{
public:
  BDSFieldMagMuonSpoiler(BDSMagnetStrength* const strength,
			 const G4double           brho);

  virtual ~BDSFieldMagMuonSpoiler(){;}

  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

private:
  G4double bField;
};

#endif
