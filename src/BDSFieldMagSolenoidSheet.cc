/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSDebug.hh"
#include "BDSFieldMagSolenoidSheet.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include "Math/SpecFuncMathMore.h"

#include <cmath>

#ifndef ROOT_HAS_MATHMORE
#include "BDSDebug.hh"
#include "BDSWarning.hh"
#endif

BDSFieldMagSolenoidSheet::BDSFieldMagSolenoidSheet(BDSMagnetStrength const* strength,
						   G4double radiusIn):
  BDSFieldMagSolenoidSheet((*strength)["length"],
			   radiusIn,
			   (*strength)["field"])
{;}

BDSFieldMagSolenoidSheet::BDSFieldMagSolenoidSheet(G4double fullLength,
						   G4double sheetRadius,
						   G4double B0In):
  a(sheetRadius*0.5),
  halfLength(0.5*fullLength),
  B0(B0In),
  Iprime(0),
  normalisation(1)
{
  finiteStrength = BDS::IsFinite(B0In);
  G4double f1 = halfLength / std::sqrt(halfLength*halfLength + a*a);
  Iprime = B0 / (CLHEP::mu0 * f1);
  
  normalisation = B0 / fullLength / CLHEP::tesla; // = mu_0 N I
#ifndef ROOT_HAS_MATHMORE
  BDS::Warning(__METHOD_NAME__, "ROOT does not include MathMore package so this field will not work");
#endif
}

#ifndef ROOT_HAS_MATHMORE
G4ThreeVector BDSFieldMagSolenoidSheet::GetField(const G4ThreeVector& /*position*/,
						 const G4double       /*t*/) const
{
  return G4ThreeVector();
}
#else
G4ThreeVector BDSFieldMagSolenoidSheet::GetField(const G4ThreeVector& position,
						 const G4double       /*t*/) const
{
  G4double z = position.z();
  G4double rho = position.perp();
  G4double phi = position.phi(); // angle about z axis

  G4double bzPlusHL = 0;
  G4double brhoPlusHL = 0;
  bzbr(rho, z+halfLength, bzPlusHL, brhoPlusHL);
  G4double bzMinusHL = 0;
  G4double brhoMinusHL = 0;
  bzbr(rho, z-halfLength, bzMinusHL, brhoMinusHL);
  
  G4double Bz = bzPlusHL - bzMinusHL;
  G4double Br = brhoPlusHL - brhoMinusHL;

  // we have to be consistent with the phi we calculated at the beginning,
  // so unit rho is in the x direction.
  G4ThreeVector result = G4ThreeVector(Br,0,Bz);
  result = result.rotateZ(phi);
  return result;
}
#endif

void BDSFieldMagSolenoidSheet::bzbr(G4double rho, G4double zeta,
				    G4double& bz, G4double& brho) const
{
  G4double fourARho = 4*a*rho;
  
  G4double aplusrhoSq = std::pow(a + rho, 2);
  G4double hSq = fourARho / aplusrhoSq;
  G4double kSq = fourARho / (aplusrhoSq + zeta*zeta);
  G4double k    = std::sqrt(kSq);

  // elliptical integrals
  G4double KofkSq  = ROOT::Math::comp_ellint_1(kSq);
  G4double EofkSq  = ROOT::Math::comp_ellint_2(kSq);
  G4double PiofhSqkSq = ROOT::Math::comp_ellint_3(hSq, kSq);
  //G4double PiofhSqkSq = ROOT::Math::comp_ellint_3(kSq, hSq);

  // terms of equations
  G4double f1 = (2.0 / (2*halfLength) ) * std::sqrt(a / rho);
  G4double f2 = ((kSq - 2) / k) * KofkSq;
  G4double f3 = 2*EofkSq / k;
  
  brho = f1 * f2 * (f2 + f3) * normalisation;
  
  G4double f4 = 1.0 / (2*halfLength);
  G4double f5 = 1.0 / std::sqrt(a * rho);
  G4double f6 = zeta*k;
  G4double f7 = KofkSq;
  G4double f8 = ( (a - rho) / (a + rho) ) * PiofhSqkSq;
  
  bz = f4 * f5 * f6 * (f7 + f8) * normalisation;
}
