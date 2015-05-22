#ifndef BDS_SBEND_FIELD_HH
#define BDS_SBEND_FIELD_HH

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "BDSMagField.hh"
#include "G4Mag_EqRhs.hh"
#include "BDSUniformMagField.hh"

class BDSSbendMagField : public BDSUniformMagField 
{
public:  // with description
  
  BDSSbendMagField(const G4ThreeVector& FieldVector,const G4double aLength,
		    const G4double aAngle);
  // A field with value equal to FieldVector.
		       
  ~BDSSbendMagField() ;

  G4double GetLocalRadius();

private:
  G4double itsLocalRadius ;

};

inline G4double BDSSbendMagField::GetLocalRadius()
{return itsLocalRadius;}

#endif
