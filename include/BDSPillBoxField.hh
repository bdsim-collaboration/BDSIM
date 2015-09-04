#include "G4ElectroMagneticField.hh"
#include "G4Types.hh"



class BSDPillBoxField: public G4ElectroMagneticField
{
public:
  BDSPillBoxField(G4double EFieldMax, G4double cavityRadius);
  ~BDSPillBoxField();
  virtual G4bool DoesFieldChangeEnergy() const;
  virtual void GetFieldValue(const G4double Point[4],G4double *Bfield ) ;
  
protected:
  G4double cavityRadius;
  G4double frequency; //angular frequency
private:
  
}
