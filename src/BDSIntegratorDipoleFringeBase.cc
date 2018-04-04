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
#include "BDSDebug.hh"
#include "BDSFieldMagDipole.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorDipoleFringeBase.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <cmath>

G4double BDSIntegratorDipoleFringeBase::thinElementLength = -1; // unphysical

BDSIntegratorDipoleFringeBase::BDSIntegratorDipoleFringeBase(BDSMagnetStrength const* strengthIn,
							     G4double                 brhoIn,
							     G4Mag_EqRhs*             eqOfMIn,
							     G4double                 minimumRadiusOfCurvatureIn,
							     const G4Transform3D&     tiltOffsetIn):
  BDSIntegratorDipoleRodrigues2(eqOfMIn, minimumRadiusOfCurvatureIn),
  polefaceAngle((*strengthIn)["polefaceangle"]),
  fringeCorr(BDS::FringeFieldCorrection(strengthIn)),
  secondFringeCorr(BDS::SecondFringeFieldCorrection(strengthIn)),
  rho(std::abs(brhoIn)/(*strengthIn)["field"]),
  fieldArcLength((*strengthIn)["length"]),
  fieldAngle((*strengthIn)["angle"]),
  tiltOffset(tiltOffsetIn),
  antiTiltOffset(tiltOffset.inverse())
{
  if (thinElementLength < 0)
    {thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();}

  zeroStrength = !BDS::IsFinite((*strengthIn)["field"]); // no fringe if no field
  BDSFieldMagDipole* dipoleField = new BDSFieldMagDipole(strengthIn);
  unitField = (dipoleField->FieldValue()).unit();
  delete dipoleField;
}

void BDSIntegratorDipoleFringeBase::BaseStepper(const G4double  yIn[6],
                                                const G4double  dydx[6],
                                                const G4double& h,
                                                G4double        yOut[6],
                                                G4double        yErr[6],
                                                const G4double& fcof,
                                                const G4double& momScaling)
{
  // Protect against neutral particles, and zero field: drift through.
  if (!BDS::IsFinite(fcof) || zeroStrength)
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      FudgeDistChordToZero(); // see doxygen in header for explanation
      return;
    }

  // container for dipole step output, used as fringe step input
  G4double yTemp[7];

  // do the dipole kick using base class
  BDSIntegratorDipoleRodrigues2::Stepper(yIn, dydx, h, yTemp, yErr); // yErr is correct output variable

  // only apply the kick if we're taking a step longer than half the length of the item,
  // in which case, apply the full kick. This appears more robust than scaling the kick
  // by h / thinElementLength as the precise geometrical length depends on the geometry
  // ie if there's a beam pipe etc -> more length safetys.  The geometry layout should
  // prevent more than one step begin taken, but occasionally, a very small initial step
  // can be taken resulting in a double kick.
  G4double lengthFraction = h / thinElementLength;

  // don't do fringe kick if we're sampling the field for a long step
  // or if it's a half step inside the thin element apply the dipole
  // motion but not the one-off fringe kick
  if ((h > 1*CLHEP::cm) || (lengthFraction < 0.501))
    {
      // copy output from dipole kick output
      for (G4int i = 0; i < 3; i++)
        {
          yOut[i]     = yTemp[i];
          yOut[i + 3] = yTemp[i + 3];
        }
      return;
    }

  // position and momentum post dipole kick.
  G4ThreeVector pos = G4ThreeVector(yTemp[0], yTemp[1], yTemp[2]);
  G4ThreeVector mom = G4ThreeVector(yTemp[3], yTemp[4], yTemp[5]);

  BDSStep  localPosMom    = GlobalToCurvilinear(pos, mom, h, true);
  G4ThreeVector localPos  = localPosMom.PreStepPoint();
  G4ThreeVector localMom  = localPosMom.PostStepPoint();
  G4ThreeVector localMomU = localMom.unit();

  // check for forward going paraxial particles - only
  if (localMomU.z() < 0.9)
    {// copy output from dipole kick output
      for (G4int i = 0; i < 3; i++)
	{
	  yOut[i]     = yTemp[i];
	  yOut[i + 3] = yTemp[i + 3];
	}
      return;
    }

  // calculate new position and momentum kick
  G4ThreeVector localCLPosOut;
  G4ThreeVector localCLMomOut;

  // normalise to particle charge
  G4double charge = fcof / std::abs(fcof);
  G4double bendingRad = rho * charge;

  // normalise to momentum - passed in from derived class.
  bendingRad *= momScaling;

  // apply fringe field kick
  OneStep(localPos, localMom, localMomU, localCLPosOut, localCLMomOut, bendingRad);

  // convert to global coordinates for output
  BDSStep globalOut = CurvilinearToGlobal(fieldArcLength, unitField, fieldAngle,
					  localCLPosOut, localCLMomOut, false, fcof,
					  tiltOffset);
  G4ThreeVector globalMom = ConvertAxisToGlobal(localCLMomOut, true);
  G4ThreeVector globalPosOut = globalOut.PreStepPoint();
  G4ThreeVector globalMomOut = globalOut.PostStepPoint();

  // error along direction of travel really
  G4ThreeVector globalMomOutU = globalMomOut.unit();
  globalMomOutU *= 1e-8;

  // write out values and errors
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]     = pos[i];
      yOut[i + 3] = globalMom[i];
      yErr[i]     = globalMomOutU[i];
      yErr[i + 3] = 1e-40;
  }
}

void BDSIntegratorDipoleFringeBase::OneStep(const G4ThreeVector& posIn,
                                            const G4ThreeVector& momIn,
                                            const G4ThreeVector& momUIn,
                                            G4ThreeVector&       posOut,
                                            G4ThreeVector&       momOut,
                                            const G4double&      bendingRadius) const
{
  G4ThreeVector posTransformed(posIn);
  G4ThreeVector momUTransformed(momUIn);
  if (tiltOffset != G4Transform3D::Identity)
    {
      auto rot = tiltOffset.getRotation();
      posTransformed.transform(rot);
      momUTransformed.transform(rot);
    }
  G4double x0 = posTransformed.x() / CLHEP::m;
  G4double y0 = posTransformed.y() / CLHEP::m;
  G4double s0 = posTransformed.z();
  G4double xp = momUTransformed.x();
  G4double yp = momUTransformed.y();
  G4double zp = momUTransformed.z();

  G4double x1 = x0;
  G4double y1 = y0;
  G4double s1 = s0;
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  G4double X11=0,X12=0,X21=0,X22 = 0;
  G4double Y11=0,Y12=0,Y21=0,Y22 = 0;

  // fringe field correction using normalised rho
  G4double fringeFieldCorrection = (fringeCorr/bendingRadius) * (1.0 - secondFringeCorr/bendingRadius);

  // calculate fringe field kick
  X11 = 1;
  X21 = std::tan(polefaceAngle) / (bendingRadius / CLHEP::m);
  X22 = 1;

  Y11 = 1;
  Y21 = -std::tan(polefaceAngle - fringeFieldCorrection) / (bendingRadius / CLHEP::m);
  Y22 = 1;

  x1  = X11*x0 + X12*xp;
  xp1 = X21*x0 + X22*xp;
  y1  = Y11*y0 + Y12*yp;
  yp1 = Y21*y0 + Y22*yp;

  // relies on normalised momenta otherwise this will be nan.
  zp1 = std::sqrt(1 - xp1*xp1 - yp1*yp1);
  if (std::isnan(zp1))
    {zp1 = zp;} // ensure not nan

  G4ThreeVector momOutUnit = G4ThreeVector(xp1, yp1, zp1);
  G4double momInMag = momIn.mag();
  momOut = momOutUnit * momInMag;

  posOut = G4ThreeVector(x1, y1, s1);

  if (tiltOffset != G4Transform3D::Identity)
    {
      auto rot = antiTiltOffset.getRotation();
      momOut.transform(rot);
      posOut.transform(rot);
    }
}


// fringe field correction terms are BDS namespace methods instead of class methods
// as the functions are called during BDSIntegratorDipoleFringeBase construction.
// The fringe field correction terms should be cached at construction rather than
// calculated every time the integrator is called.

G4double BDS::FringeFieldCorrection(BDSMagnetStrength const* strength)
{
  G4double hgap    = (*strength)["hgap"];
  G4double fint    = (*strength)["fringeint"];
  G4double pfAngle = (*strength)["polefaceangle"];
  G4double vertGap = 2 * hgap;
  G4double corrValue = fint * vertGap * (1.0 + std::pow(sin(pfAngle),2)) / cos(pfAngle);
  return corrValue;
}

G4double BDS::SecondFringeFieldCorrection(BDSMagnetStrength const* strength)
{
  G4double hgap    = (*strength)["hgap"];
  G4double fint    = (*strength)["fringeint"];
  G4double fintK2  = (*strength)["fringeintK2"];
  G4double pfAngle = (*strength)["polefaceangle"];
  G4double vertGap = 2 * hgap;
  G4double corrValue = fint * fintK2 * vertGap * tan(pfAngle);
  return corrValue;
}
