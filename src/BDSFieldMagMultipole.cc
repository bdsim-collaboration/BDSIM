#include "BDSDebug.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"

#include <cmath>
#include <vector>

BDSFieldMagMultipole::BDSFieldMagMultipole(const BDSMagnetStrength* strength,
					   const G4double           brho,
					   const G4int              orderIn = 12):
  order(orderIn)
{
  normalComponents = strength->GetNormalComponents();
  skewCompoennts   = strength->GetSkewComponents();
  // multiple by brho to get field coefficients
  for (auto kn : normalComponents)
    {kn *= brho;}
  for (auto ksn : skewCompoennts)
    {ksn *= brho;}
  
  // safety check - ensure we're not going to a higher order than the strength
  // class supports.
  if (std::abs(order) > normalComponents.size())
    {order = (G4int)normalComponents.size();}
}

void BDSFieldMagMultipole::GetFieldValue(const G4double point[4],
					 G4double* field) const
{
  G4ThreeVector localPosition = ConvertToLocal(point);

  // Translate carteasian to polar coordinates
  G4double r   = sqrt(pow(localPosition.x(),2) + pow(localPosition.y(),2));
  G4double phi = 0;
  if (BDS::IsFinite(std::abs(r)))
    {phi = atan2(localPosition.y(),localPosition.x());}

  // compute B field in local coordinates
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
  for (G4int i = 0; it < order; i++)
    {
      G4double o = (G4double)i+1; // the current order
      br   += normalComponents[i] * pow(r, o - 1) * sin(o * phi) / ffact; //normal
      br   -= skewComponents[i]   * pow(r, o - 1) * cos(o * phi) / ffact; //skewed

      bphi += normalComponents[i] * pow(r, o - 1) * cos(o * phi) / ffact; //normal
      bphi += skewComponents[i]   * pow(r, o - 1) * sin(o * phi) / ffact; //skewed

      if(o==1) // for the angle convention
	{br *= -1; bphi *=-1; }

      ffact *= o;
    }

  G4ThreeVector localField;
  localField[0] = (br * cos(phi) - bphi * sin(phi)); // B_x
  localField[1] = (br * sin(phi) + bphi * cos(phi)); // B_y
  localField[2] = 0;                                 // B_z

  OutputToGlobal(localField, field);
}



