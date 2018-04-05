/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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

#include "BDSIntegratorDipoleFringeBase.hh"
#include "BDSIntegratorDipoleFringe.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Derived fringe field integrator that does not normalise to momentum.
 *
 * @author Will Shields 
 */

class BDSIntegratorDipoleFringe: public BDSIntegratorDipoleFringeBase
{
public:
  BDSIntegratorDipoleFringe(BDSMagnetStrength const* strength,
			    G4double                 brhoIn,
			    G4Mag_EqRhs*             eqOfMIn,
			    G4double                 minimumRadiusOfCurvature,
			    const G4Transform3D&     tiltOffsetIn = G4Transform3D::Identity);
  
  virtual ~BDSIntegratorDipoleFringe(){;}

  /// The stepper for integration. Calls base class stepper.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleFringe() = delete;
};

#endif
