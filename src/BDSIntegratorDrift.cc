#include "BDSIntegratorDrift.hh"

#include "G4ThreeVector.hh"

BDSIntegratorDrift::BDSIntegratorDrift()
{;}

BDSIntegratorDrift::~BDSIntegratorDrift()
{;}

void BDSIntegratorDrift::AdvanceDriftMag(const G4double yIn[],
					 const G4double h,
					 G4double       yOut[],
					 G4double*      yErr) const
{
  G4ThreeVector unitMomentum = G4ThreeVector(yIn[3],yIn[4],yIn[5]).unit();

  AdvanceDriftMag(yIn, unitMomentum, h, yOut, yErr);
}

void BDSIntegratorDrift::AdvanceDriftMag(const G4double       yIn[],
					 const G4ThreeVector& unitMomentum,
					 const G4double       h,
					 G4double             yOut[],
					 G4double*            yErr) const
{
  G4bool writeError = yErr ? true : false;
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = yIn[i] + h * unitMomentum[i]; // update position
      yOut[i+3] = yIn[i+3];                   // momentum doesn't change
      if (writeError)
	{
	  yErr[i]   = 0;
	  yErr[i+3] = 0;
	}
    }
}
