#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSFieldMagSBend.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"

#include <limits>
#include <cmath>

/*
BDSFieldMagSBend::BDSFieldMagSBend(G4ThreeVector fieldIn,
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
	  G4double momentum   = BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV;
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
*/

BDSFieldMagSBend::BDSFieldMagSBend(BDSMagnetStrength* strength,
				   G4double           brho,
				   G4ThreeVector      unitDirection)
{
  G4double charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();
  G4double ffact  = BDSGlobalConstants::Instance()->GetFFact();
  // note the direction of the bend shouldn't change with magnet flip with charge, hence
  // the multiplacation by ffact
  G4double bFieldMag = brho * (*strength)["angle"]/(*strength)["length"] * charge * ffact;
  G4double unitMag   = unitDirection.mag();
  if (!BDS::IsFinite(unitMag))
    {localField = G4ThreeVector(0, bFieldMag, 0);}
  else
    {localField = bFieldMag * unitDirection.unit();}
  // always normalise to ensure it's a unit vector
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << localBField << G4endl;
#endif
}

void BDSFieldMagSBend::GetFieldValue(const G4double point[4],
				     G4double* field) const
{
  ConvertToLocal(point);

  OutputToGlobal(localField, field);
}
