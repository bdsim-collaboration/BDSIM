#include "BDSDetectorSolenoidMagField.hh"
#include "BDSMagFieldMesh.hh"

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

BDSDetectorSolenoidMagField::BDSDetectorSolenoidMagField(G4double BIn,
							 G4double BOut,
							 G4double radiusIn,
							 G4double radiusOut,
							 G4double zMin,
							 G4double zMax):
  itsBIn(BIn),
  itsBOut(BOut),
  itsRadiusIn(radiusIn),
  itsRadiusOut(radiusOut),
  itsZMin(zMin),
  itsZMax(zMax)
{;}

BDSDetectorSolenoidMagField::~BDSDetectorSolenoidMagField()
{;}

void BDSDetectorSolenoidMagField::GetFieldValue(const G4double Point[4],
						G4double *Bfield) const
{ 
  G4ThreeVector     GlobalR      = G4ThreeVector(Point[0], Point[1], Point[2]);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector     LocalR       =GlobalAffine.TransformPoint(GlobalR); 

  Bfield[0] = 0;
  Bfield[1] = 0;

  G4double zField = 0;

  G4double localRad = sqrt(pow(LocalR.y(),2)+pow(LocalR.x(),2));

  if( (LocalR.z() > itsZMin) && (LocalR.z() < itsZMax) )
    {
      if(localRad<itsRadiusIn)
	{zField = itsBIn;}
      else if(localRad<itsRadiusOut)
	{zField = itsBOut;}
      else zField=0;
    }
  else
    {zField=0;}

  Bfield[2] = zField;
}



