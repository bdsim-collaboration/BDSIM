#include <limits>
#include "BDSGlobalConstants.hh"
#include "G4UniformMagField.hh"
#include "globals.hh"
#include "BDSSbendMagField.hh"

#include "BDSUtilities.hh"

BDSSbendMagField::BDSSbendMagField(const G4ThreeVector& aField,
				   const G4double length,
				   const G4double angle):
  G4UniformMagField(aField)
{
  if (BDS::IsFinite(angle))
    { // original
      //    itsLocalRadius=length/angle;
      itsLocalRadius=-length/angle;// minus sign for correct machine convention
      // check for synchrotron radiation factors
#ifdef BDSDEBUG
      G4double B_inferred= 
        (BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV)/
        (0.299792458 * (CLHEP::GeV/CLHEP::tesla/CLHEP::m)*itsLocalRadius/CLHEP::m);
      G4cout<<"B_inferred="<<B_inferred/CLHEP::tesla<<
	" aField="<<aField/CLHEP::tesla<<G4endl;
#endif
    }
  else
    {
      itsLocalRadius = std::numeric_limits<double>::max();
#ifdef BDSDEBUG
      G4double B_inferred=0;
      G4cout<<"B_inferred="<<B_inferred/CLHEP::tesla<<
	" aField="<<aField/CLHEP::tesla<<G4endl;
#endif
    }
}

BDSSbendMagField::~BDSSbendMagField()
{}


