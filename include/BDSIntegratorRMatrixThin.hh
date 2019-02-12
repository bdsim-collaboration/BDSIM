/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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

#ifndef BDSINTEGRATORRMATRIXTHIN_HH
#define BDSINTEGRATORRMATRIXTHIN_HH

#include "BDSIntegratorMag.hh"
#include "BDSMagUsualEqRhs.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;
class BDSParticleDefinition;

/**
 * @brief Integrator that applies transfer matrix effect to particles passing through a thin element
 *
 * If the new particle x,y coordinates are greater than maximumRadius, they are clipped
 * to this value.
 *
 * @author Stewart Boogert
 */

class BDSIntegratorRMatrixThin: public BDSIntegratorMag
{
public:
  BDSIntegratorRMatrixThin(BDSMagnetStrength const* strength,
                           G4double                 brhoIn,
                           G4Mag_EqRhs* eqOfMIn,
                           const BDSParticleDefinition* designParticle,
                           G4double maximumRadiusIn);

  virtual ~BDSIntegratorRMatrixThin(){;}

  virtual void Stepper(const G4double y[],
                       const G4double dydx[],
                       const G4double h,
                       G4double       yOut[],
                       G4double       yErr[]);

private:
  BDSIntegratorRMatrixThin();

  G4double kick1;
  G4double kick2;
  G4double kick3;
  G4double kick4;
  G4double rmat11;
  G4double rmat12;
  G4double rmat13;
  G4double rmat14;
  G4double rmat15;
  G4double rmat16;
  G4double rmat21;
  G4double rmat22;
  G4double rmat23;
  G4double rmat24;
  G4double rmat25;
  G4double rmat26;
  G4double rmat31;
  G4double rmat32;
  G4double rmat33;
  G4double rmat34;
  G4double rmat35;
  G4double rmat36;
  G4double rmat41;
  G4double rmat42;
  G4double rmat43;
  G4double rmat44;
  G4double rmat45;
  G4double rmat46;
  G4double rmat51;
  G4double rmat52;
  G4double rmat53;
  G4double rmat54;
  G4double rmat55;
  G4double rmat56;
  G4double rmat61;
  G4double rmat62;
  G4double rmat63;
  G4double rmat64;
  G4double rmat65;
  G4double rmat66;

  const G4double    nominalBRho;  ///< Cached magnet property, nominal magnetic rigidity
  BDSMagUsualEqRhs* eq;
  const G4double    nominalEnergy;  ///< Nominal beam energy
  const	G4double    nominalMass;    ///< Primary particle mass. Needed for recalculating nominal energy with scaling.

  G4double maximumRadius;

  /// General integrator that can be used as a backup if the particle momentum is
  /// outside the (transverse) momentum range applicable for the integration scheme
  /// used by the derived integrator.
  G4MagIntegratorStepper* backupStepper;


  /// Keep a reference to the underlying equation of motion, but through a higher
  /// level pointer than G4EquationOfMotion* so we can use the correct methods. This
  /// class doesn't own this.
  //G4EqMagElectricField* eqOfM;

  /// Whether a magnet has a finite strength or not. Can be set in the constructor for
  /// zero strength elements and then a drift routine is used before anything else.
  G4bool zeroStrength;

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChordPrivate;

  /// Cache of thin element length to know maximum possible length scale step
  /// for coordinate lookup.
//  static G4double thinElementLength;

  /// Cache of the fraction of the momentum outside which don't use a matrix
  /// as it's just not feasible.
//  static G4double nominalMatrixRelativeMomCut;

  /// Setter for distChord to private member.
  inline void SetDistChord(G4double distChordIn) {distChordPrivate = distChordIn;}

};

#endif
