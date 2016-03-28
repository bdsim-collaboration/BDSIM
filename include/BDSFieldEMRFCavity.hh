#include "BDSFieldEM.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

class BDSFieldEMRFCavity: public BDSFieldEM
{
public:
  BDSFieldEMRFCavity(G4double EFieldMax,
		     G4double cavityRadius,
		     G4double frequency,
		     G4double phase);
  virtual ~BDSFieldEMRFCavity(){;}
  virtual G4bool DoesFieldChangeEnergy() const;

  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector &position,
														   const G4double t) const;
  
protected:
  /// Maximum field in MV/m
  G4double eFieldMax;
  G4double cavityRadius;
  /// angular frequency
  G4double frequency;
  G4double phase;
};
