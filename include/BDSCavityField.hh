#include "G4ElectroMagneticField.hh"
#include "G4Types.hh"
#include "G4Navigator.hh"
#include "BDSAuxiliaryNavigator.hh"



class BDSCavityField:
  public G4ElectroMagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSCavityField(G4double EFieldMax,
		  G4double cavityRadius,
		  G4double frequency,
		  G4double phase);
  ~BDSCavityField();
  virtual G4bool DoesFieldChangeEnergy() const;
  virtual void GetFieldValue(const G4double Point[4],G4double *Bfield ) const ;
  
protected:
  /// Maximum field in MV/m
  G4double eFieldMax;
  G4double cavityRadius;
  /// angular frequency
  G4double frequency;
  G4double phase;
private:
  //G4Navigator* PillBoxNavigator;
};
