#include "BDSPillBoxField.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>
#include "globals.hh"

BDSPillBoxField::BDSPillBoxField(G4double EFieldMaxIn,G4double cavityRadius)
{
  frequency = 2.405*CLHEP::c_light/cavityRadius;
  EFieldMax = EFieldMaxIn;
};

G4bool BDSPillBoxField::DoesFieldChangeEnergy() const
{
  //In general EM field will alter particle energy
  return true;
};
//global to local transfomrations?  learn more about this...
void BDSPillBoxField::GetFieldValue(const G4double Point[4],G4double *Bfield )
{
  //Point[4] is an array of global coordinates (unfortunately)
  //Need to convert to local coordinates.
  //B_theta = (EFieldMax/c)*J_1(k_r*r)*sin(omega t)
  Bfield[0] = 
  Bfield[1] = 
  Bfield[2] = 0; //B Field has no z-component.
  //E_z = EFieldMax*J_0(k_r*r) * cos(omega t)
  Bfield[3] = 0; //EField only along z-axis
  Bfield[4] = 0; //Efield only along z-axis
  Bfield[5] = EFieldMax*/*BesselFunctionFirstKind**/cos(frequency*Point[3]);
  
  //  Return as Bfield[0], [1], [2] the magnetic field x, y & z components
  //   and   as Bfield[3], [4], [5] the electric field x, y & z components
}

BDSPillBoxField::~BDSPillBoxField(){};
