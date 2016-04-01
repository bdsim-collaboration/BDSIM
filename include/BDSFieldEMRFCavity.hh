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
  virtual G4bool DoesFieldChangeEnergy() const {return true;}

  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector &position,
														   const G4double t) const;
  
private:
  /// Private constructor to force use of provided one.
  BDSFieldEMRFCavity();
  
  /// Maximum field in MV/m
  G4double eFieldMax;
  G4double cavityRadius;
  
  /// angular frequency
  G4double frequency;
  G4double phase;

  /// x coordinate of first 0 point for bessel J0.
  static const G4double j0FirstZero;
};
