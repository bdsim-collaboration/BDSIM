/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSINTEGRATORDIPOLEFRINGE_H
#define BDSINTEGRATORDIPOLEFRINGE_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDipole2.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution for a dipole kick.
 *
 * @author Will Shields 
 */

class BDSIntegratorDipoleFringe: public BDSIntegratorDipole2, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorDipoleFringe(BDSMagnetStrength const* strength,
                G4double                 brhoIn,
			    G4Mag_EqRhs*             eqOfMIn,
			    G4double                 minimumRadiusOfCurvature);
  
  virtual ~BDSIntegratorDipoleFringe(){;}

  /// The stepper for integration. Uses BDSIntegratorDipole2::Stepper and then adds
  /// a kick in yp in curvilinear coordinates.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);
  
private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleFringe() = delete;
  
  /// Poleface rotation angle
  const G4double polefaceAngle;
  /// Fringe field correction term
  const G4double fringeCorr;
  /// Brho for momentum normalisation
  const G4double brho;

  /// Cache of thin element length from global constants. Initialised via check
  /// on unphysical -1 value as global constants doesn't exist at compile time.
  static G4double thinElementLength;
};

#endif
