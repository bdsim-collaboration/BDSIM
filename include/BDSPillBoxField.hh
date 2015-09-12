#include "G4ElectroMagneticField.hh"
#include "G4Types.hh"
#include "G4Navigator.hh"
#include "BDSAuxiliaryNavigator.hh"



class BDSPillBoxField:
  public G4ElectroMagneticField, public BDSAuxiliaryNavigator
{
public:
  BDSPillBoxField(G4double EFieldMax,
		  G4double cavityRadius,
		  G4double frequency,
		  G4double phase);
  ~BDSPillBoxField();
  virtual G4bool DoesFieldChangeEnergy() const;
  virtual void GetFieldValue(const G4double Point[4],G4double *Bfield ) const ;
  
protected:
  G4double cavityRadius;
  G4double frequency; //angular frequency
  G4double phase;
  G4double eFieldMax;
private:
  //G4Navigator* PillBoxNavigator;
};
