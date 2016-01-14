#include "BDSDebug.hh"
#include "BDSFieldMagDecapole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include <cmath>

BDSFieldMagDecapole::BDSFieldMagDecapole(const BDSMagnetStrength* strength,
					 const G4double           brho)
{
  // B'''' = d^4By/dx^4 = Brho * (1/Brho d^4By/dx^4) = Brho * k4
  bQuadruplePrime = brho * (*strength)["k4"];
  bQPNormed       = bQuadruplePrime / 24.;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'''' = " << bQuadruplePrime << G4endl;
#endif
}

void BDSFieldMagDecapole::GetFieldValue(const G4double point[4],
					G4double* field) const
{
  G4ThreeVector localPosition = ConvertToLocal(point);

  // B_x = 4xy(x^2-y^2) * ( B''''/4!)
  // B_y = (x^4 - 6x^2y^2 + y^4) * ( B''''/4!)
  // B_z = 0
  
  //shortcuts
  G4double x = localPosition.x();
  G4double y = localPosition.y();

  G4ThreeVector localField;
  localField[0] = 4 * x * y * (pow(x,2)-pow(y,2)) * bQPNormed;
  localField[1] = (pow(x,4) - 6 * pow(x,2) * pow(y,2) + pow(y,4)) * bQPNormed;
  localField[2] = 0;

  OutputGlobalField(localField, field);
}



