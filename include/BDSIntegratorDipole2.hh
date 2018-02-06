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
#ifndef BDSINTEGRATORDIPOLE2_H
#define BDSINTEGRATORDIPOLE2_H

#include "BDSIntegratorDrift.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4MagHelicalStepper.hh"
#include "G4ThreeVector.hh"

class G4MagIntegratorStepper;
class G4Mag_EqRhs;

/**
 * @brief Exact helix through pure dipole field.
 *
 * Integrator based on G4MagHelicalStepper that provides required
 * tracking througha pure magnetic field, but is a pure virtual class.
 * 
 * This class implements the required Stepper method that uses the provided
 * AdvanceHelix method from the base class.
 *
 * Unlike other BDSIM integrators this works in global coordinates and does
 * not require the transforms to curvilinear coordinates.
 *
 * For low momentum particles (|p| < 40 MeV) we use the G4ClassicalRK4 integrator
 * instead of the one for the pure magnetic field as it is more robust.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorDipole2: public G4MagHelicalStepper, public BDSIntegratorDrift
{
public:
  BDSIntegratorDipole2(G4Mag_EqRhs* eqOfMIn,
		       G4double     minimumRadiusOfCurvature);

  virtual ~BDSIntegratorDipole2(){;}

  /// Required to be provided by base class, but apparently should never be
  /// called by the driver.  Simply calls AdvanceHelix.
  virtual void DumbStepper(const G4double yIn[],
			   G4ThreeVector  field,
			   G4double       stepLength,
			   G4double       yOut[]);

  /// Calculate output coordinates.  Decide if particle is spiralling or not.
  /// Nominally calculate two half steps and compare to one full step for error
  /// estimation.
  virtual void Stepper(const G4double yInput[],  ///< Input coordinates x,y,z,px,py,pz,t
		       const G4double dydyx[],   ///< Partial differentials at yInput
		       G4double       stepLength,///< Length of trajectory to calculate
		       G4double       yOut[],    ///< Output array
		       G4double       yErr[]);   ///< Output error array

  virtual G4int IntegratorOrder() const {return 1;}

  /// Variation of AdvanceHelix specifically to deal with particles that are likely to
  /// be spiralling in the magnetic field.
  void AdvanceHelixForSpiralling(const G4double yIn[],
				 G4ThreeVector  field,
				 G4double       stepLength,
				 G4double       yOut[],
				 G4double       yErr[]);

  /// Public accessor for protected variable in base class.
  inline G4double RadiusOfHelix() const {return GetRadHelix();}

protected:
  /// DistChord() is non-virtual function in base class so set Ang and RadHelix appropriately
  /// such that DistChord() will return 0.
  void FudgeDistChordToZero();

  /// Cache of equation of motion. This class does not own it.
  G4Mag_EqRhs* eqOfM;

private:
  /// Private default constructor to force use of provided one.
  BDSIntegratorDipole2() = delete;

  /// The minimum tolerable radius of curvature before we decide the particle is
  /// spiralling and should be treated differently.
  G4double minimumRadiusOfCurvature;

};

#endif
