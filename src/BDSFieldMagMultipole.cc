#include "BDSDebug.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <vector>

BDSFieldMagMultipole::BDSFieldMagMultipole(BDSMagnetStrength const* strength,
					   G4double          const  brho,
					   G4int             const  orderIn):
  order(orderIn)
{
  normalComponents = strength->NormalComponents();
  skewComponents   = strength->SkewComponents();
  // multiple by brho to get field coefficients
  for (auto kn : normalComponents)
    {kn *= brho;}
  for (auto ksn : skewComponents)
    {ksn *= brho;}
  
  // safety check - ensure we're not going to a higher order than the strength
  // class supports.
  if (std::abs(order) > (G4int)normalComponents.size())
    {order = (G4int)normalComponents.size();}
}

G4ThreeVector BDSFieldMagMultipole::GetField(const G4ThreeVector &position) const
{
  // Translate carteasian to polar coordinates
  G4double r   = sqrt(pow(position.x(),2) + pow(position.y(),2));
  G4double phi = 0;
  if (BDS::IsFinite(std::abs(r)))
    {phi = atan2(position.y(),position.x());}

  // compute B field in cylindrical coordinates first then convert to carteasian
  // Bx = Br * cos(phi) - Bphi * sin(phi)
  // By = Br * sin(phi) + Bphi * cos(phi)
  // where, if n=1 is for dipole:
  // Br  (n) (normal) = +Bn/(n-1)! * r^(n-1) * sin(n*phi)
  // Bphi(n) (normal) = +Bn/(n-1)! * r^(n-1) * cos(n*phi)
  // Br  (n) (skewed) = +Bn/(n-1)! * r^(n-1) * cos(n*phi)
  // Bphi(n) (skewed) = -Bn/(n-1)! * r^(n-1) * sin(n*phi)
  // cumulative variables for calculating the field

  G4double br = 0;
  G4double bphi = 0;
  // I want to use the strange convention of dipole coeff. with opposite sign -
  // then it is the same sign as angle.
  G4double ffact = -1;
  for (G4int i = 0; i < order; i++)
    {
      // Here we add to so order represents kn properly
      G4double o = (G4double)i+2; // the current order
      br   += normalComponents[i] * pow(r, o - 1) * sin(o * phi) / ffact; //normal
      br   -= skewComponents[i]   * pow(r, o - 1) * cos(o * phi) / ffact; //skewed

      bphi += normalComponents[i] * pow(r, o - 1) * cos(o * phi) / ffact; //normal
      bphi += skewComponents[i]   * pow(r, o - 1) * sin(o * phi) / ffact; //skewed

      // Ignore dipole component for now!
      //if(o==1) // for the angle convention
	  //{br *= -1; bphi *=-1; }

      ffact *= o;
    }

  G4ThreeVector carteasianField;
  carteasianField[0] = (br * cos(phi) - bphi * sin(phi)); // B_x
  carteasianField[1] = (br * sin(phi) + bphi * cos(phi)); // B_y
  carteasianField[2] = 0;                                 // B_z

  return carteasianField;
}



