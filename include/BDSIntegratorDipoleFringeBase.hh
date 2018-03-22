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
#ifndef BDSINTEGRATORDIPOLEFRINGEBASE_H
#define BDSINTEGRATORDIPOLEFRINGEBASE_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDipoleRodrigues2.hh"
#include "BDSIntegratorDipoleQuadrupole.hh"

#include "globals.hh"

namespace BDS
{
  /// Function to calculate the value of the fringe field correction term.
  G4double FringeFieldCorrection(BDSMagnetStrength const* strength);

  /// Function to calculate the value of the second fringe field correction term.
  G4double SecondFringeFieldCorrection(BDSMagnetStrength const* strength);
}

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution for a dipole kick.
 *
 * @author Will Shields 
 */

class BDSIntegratorDipoleFringeBase: public BDSIntegratorDipoleRodrigues2, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorDipoleFringeBase(BDSMagnetStrength const* strength,
				G4double                 brhoIn,
				G4Mag_EqRhs*             eqOfMIn,
				G4double                 minimumRadiusOfCurvature);
  
  virtual ~BDSIntegratorDipoleFringeBase(){;}

  /// The stepper for integration. Particle charge and momentum scaling are provided as arguments
  /// as they are calculated in the derived classes. Uses BDSIntegratorDipole2::Stepper and then adds
  /// a kick in yp in curvilinear coordinates.
  void BaseStepper(const G4double yIn[],
                   const G4double dydx[],
                   const G4double h,
                   G4double       yOut[],
                   G4double       yErr[],
                   G4double       fcof,
                   G4double       momScaling);


  /// Calculate a single step using dipole fringe field matrix.
  /// Unit momentum, momentum magnitude, and normalised bending radius are provided
  /// as arguments because they already calculated in the BaseStepper method.
  void OneStep(G4ThreeVector  posIn,
               G4ThreeVector  momIn,
               G4ThreeVector  momUIn, // assumed unit momentum of momIn
               G4ThreeVector& posOut,
               G4ThreeVector& momOut,
               G4double       bendingRadius) const;

protected:
  /// Poleface rotation angle
  const G4double polefaceAngle;
  /// Fringe field correction term
  const G4double fringeCorr;
  /// Second fringe field correction term
  const G4double secondFringeCorr;
  /// Nominal magnet bending radius
  const G4double rho;

  BDSMagnetStrength const* strength;

  /// Whether a magnet has a finite strength or not. Can be set in the constructor for
  /// zero strength elements and then a drift routine is used before anything else.
  G4bool zeroStrength;
  
  /// Cache of thin element length from global constants. Initialised via check
  /// on unphysical -1 value as global constants doesn't exist at compile time.
  static G4double thinElementLength;

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleFringeBase() = delete;
};

#endif
