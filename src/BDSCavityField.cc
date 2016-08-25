#include "BDSCavityField.hh"
#include "G4Navigator.hh"
#include "CLHEP/Units/PhysicalConstants.h" 
#include "globals.hh"

#include <cmath>

#include "TMath.h"

const G4double BDSCavityField::j0FirstZero = 2.404825557695772768622;

BDSCavityField::BDSCavityField(G4double eFieldMaxIn,
				 G4double cavityRadiusIn,
				 G4double frequencyIn,
				 G4double phaseIn):
  eFieldMax(eFieldMaxIn),
  cavityRadius(cavityRadiusIn),
  frequency(frequencyIn),
  phase(phaseIn),
  normalisedCavityRadius(j0FirstZero/cavityRadiusIn)
{;}

G4bool BDSCavityField::DoesFieldChangeEnergy() const
{
    return true;
}

void BDSCavityField::GetFieldValue(const G4double Point[4],G4double *Bfield ) const
{
  G4ThreeVector position;  //Vector to hold the local coordinates
  G4ThreeVector GlobalR; //Vector to hold the global coordinates
  //Initializing GlobalR.  (GetFieldValue is given global coordinates)
  GlobalR.setX(Point[0]); 
  GlobalR.setY(Point[1]);
  GlobalR.setZ(Point[2]);

  G4double t = Point[3];
  
  auxNavigator->LocateGlobalPointAndSetup(GlobalR); //Give navigator global coordinates
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform(); //transform to local
  position = GlobalAffine.TransformPoint(GlobalR);  //initialize position with local coords
  
  G4RotationMatrix globalToLocalMatrix = auxNavigator->NetRotation();   //Matrix of Global to Local
  G4RotationMatrix localToGlobalMatrix = globalToLocalMatrix.inverse();  //Find the inverse of this transform to be used later

  //Converting from Local Cartesian to Local Cylindrical
  G4double phi = atan2(position.y(),position.x());
  G4double r = std::sqrt(pow(position.x(),2) +  pow(position.y(),2));

  G4double rNormalised = normalisedCavityRadius * r;

  // In case a point outside the cavity is queried, ensure the bessel will return 0
  if (rNormalised > j0FirstZero)
    {rNormalised = j0FirstZero - 1e-6;}

  //Source for calculating the TM010 mode: Gerigk, Frank. "Cavity types." arXiv preprint arXiv:1111.4897 (2011).

  G4double J0r = TMath::BesselJ0(rNormalised);
  G4double J1r = TMath::BesselJ1(rNormalised);

  //Calculating free-space impedance and scale factor for Bphi:
  G4double Z0 = std::sqrt(CLHEP::mu0/CLHEP::epsilon0);
  G4double Bmax = -eFieldMax/Z0;

  //Calculating field components.  Frequency in rad/s or /s?
  G4double Ez = eFieldMax * J0r * cos(frequency*t);
  G4double Bphi = Bmax * J1r * sin(frequency*t);

  //Converting Bphi into cartesian coordinates:

  G4double Bx = Bphi*sin(phi);
  G4double By = Bphi*cos(phi);

  //Local B and E fields:
  G4ThreeVector LocalB = G4ThreeVector(Bx,                          //x
				       By,                          //y
				       0);                          //z

  G4ThreeVector LocalE = G4ThreeVector(0,                           //x
				       0,                           //y
				       Ez);                         //z
  
  //Converting back to global coordinates:
  G4ThreeVector GlobalB = localToGlobalMatrix*LocalB; //Multiply the localB field by the local2Global matrix to get back into global coordinates
  G4ThreeVector GlobalE = localToGlobalMatrix*LocalE;
  
 
  //BField x, y, z
  Bfield[0] = GlobalB.x();  
  Bfield[1] = GlobalB.y();
  Bfield[2] = GlobalB.z();
  //EField x, y, z 
  Bfield[3] = GlobalE.x();
  Bfield[4] = GlobalE.y();
  Bfield[5] = GlobalE.z();
  }

BDSCavityField::~BDSCavityField()
{
}
