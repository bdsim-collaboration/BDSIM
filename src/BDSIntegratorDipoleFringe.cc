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
#include <include/BDSGlobalConstants.hh>
#include "BDSDebug.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

BDSIntegratorDipoleFringe::BDSIntegratorDipoleFringe(BDSMagnetStrength const* strengthIn,
                             G4double                 brhoIn,
						     G4Mag_EqRhs*             eqOfMIn,
                             G4double                 minimumRadiusOfCurvatureIn):
  BDSIntegratorDipoleFringeBase(strengthIn, brhoIn, eqOfMIn, minimumRadiusOfCurvatureIn)
{;}

void BDSIntegratorDipoleFringe::Stepper(const G4double yIn[],
                                        const G4double dydx[],
                                        const G4double h,
                                        G4double       yOut[],
                                        G4double       yErr[])
{
  // unit normalisation
  const G4double fcof = eqOfM->FCof();

  BaseStepper(yIn, dydx, h, yOut, yErr, fcof, 1.0);
}

