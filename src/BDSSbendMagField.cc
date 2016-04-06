#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSbendMagField.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

#include <limits>

BDSSbendMagField::BDSSbendMagField(G4ThreeVector fieldIn,
				   const G4double length,
				   const G4double angle,
				   G4bool debugIn):
  field(fieldIn),
  debug(debugIn)
{
#ifdef BDSDEBUG
  debug = true;
#endif
  if (BDS::IsFinite(angle))
    {
      // localRadius = length/angle;
      // minus sign for correct machine convention
      localRadius = -length/angle;
      // check for synchrotron radiation factors
      if (debug)
	{
	  G4double momentum   = BDSGlobalConstants::Instance()->BeamMomentum()/CLHEP::GeV;
	  G4double B_inferred = momentum / (0.299792458 * (CLHEP::GeV/CLHEP::tesla/CLHEP::m)*localRadius/CLHEP::m);
	  G4cout  << __METHOD_NAME__ << "B inferred = " << B_inferred/CLHEP::tesla
		  << " field = " << field/CLHEP::tesla << G4endl;
	}
    }
  else
    {
      localRadius = std::numeric_limits<double>::max();
      if (debug)
	{
	  G4double B_inferred = 0;
	  G4cout << "B inferred = " << B_inferred/CLHEP::tesla
		 << " field = " << field/CLHEP::tesla << G4endl;
	}
    }
}

BDSSbendMagField::~BDSSbendMagField()
{}

void BDSSbendMagField::GetFieldValue(const G4double point[4],
				     G4double* bField) const
{
  G4ThreeVector pointVector(point[0], point[1], point[2]);
  auxNavigator->LocateGlobalPointAndSetup(pointVector);
  G4AffineTransform globalAffine = G4AffineTransform(auxNavigator->GetLocalToGlobalTransform());
  G4ThreeVector     globalField  = G4ThreeVector(field); // copy global field first to preserve constness
  globalAffine.ApplyAxisTransform(globalField); // modifies global field
  if (debug)
    {
      G4cout << __METHOD_NAME__ << "local field:  " << field       << G4endl;
      G4cout << __METHOD_NAME__ << "global field: " << globalField << G4endl;
    }
  
  bField[0] = globalField[0];
  bField[1] = globalField[1];
  bField[2] = globalField[2];
  bField[3] = 0;
  bField[4] = 0;
  bField[5] = 0;
}
