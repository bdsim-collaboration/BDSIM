#ifndef BDSFIELDMAGMUONSPOILER_H
#define BDSFIELDMAGMUONSPOILER_H

#include "BDSFieldMagBase.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals

/** 
 * @brief Field of a Muon Spoiler.
 *
 */

class BDSFieldMagMuonSpoiler: public BDSFieldMagBase
{
public:
  BDSFieldMagMuonSpoiler(BDSMagnetStrength* const strength,
			 const G4double           brho);

  ~BDSFieldMagMuonSpoiler(){;}
  
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;

private:
  G4double bField;
};

#endif
