#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSbendMagField.hh"
#include "BDSUtilities.hh"

#include "G4UniformMagField.hh"
#include "globals.hh"

#include <limits>

BDSSbendMagField::BDSSbendMagField(G4ThreeVector fieldIn,
				   const G4double length,
				   const G4double angle):
  field(fieldIn)
{
  if (BDS::IsFinite(angle))
    {
      // localRadius = length/angle;
      // minus sign for correct machine convention
      localRadius = -length/angle;
      // check for synchrotron radiation factors
#ifdef BDSDEBUG
      G4double B_inferred= 
        (BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV)/
        (0.299792458 * (CLHEP::GeV/CLHEP::tesla/CLHEP::m)*localRadius/CLHEP::m);
      G4cout<<"B_inferred="<<B_inferred/CLHEP::tesla<<
	" aField="<<field/CLHEP::tesla<<G4endl;
#endif
    }
  else
    {
      localRadius = std::numeric_limits<double>::max();
#ifdef BDSDEBUG
      G4double B_inferred=0;
      G4cout<<"B_inferred="<<B_inferred/CLHEP::tesla<<
	" aField="<<field/CLHEP::tesla<<G4endl;
#endif
    }
}

BDSSbendMagField::~BDSSbendMagField()
{}

void BDSSbendMagField::GetFieldValue(const G4double point[4],
				     G4double* bField) const
{
  G4ThreeVector pointVector(point[0], point[1], point[2]);
  auxNavigator->LocateGlobalPointAndSetup(pointVector);
  G4AffineTransform globalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector globalField = G4ThreeVector(field);
  globalAffine.ApplyAxisTransform(globalField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "local field:  " << field       << G4endl;
  G4cout << __METHOD_NAME__ << "global field: " << globalField << G4endl;
#endif
  
  bField[0] = globalField[0];
  bField[1] = globalField[1];
  bField[2] = globalField[2];
  bField[3] = 0;
}
