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
#include <include/BDSGlobalConstants.hh>
#include "BDSDebug.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

G4double BDSIntegratorDipoleFringe::thinElementLength = -1; // unphyiscal

BDSIntegratorDipoleFringe::BDSIntegratorDipoleFringe(BDSMagnetStrength const* strengthIn,
                             G4double                 brhoIn,
						     G4Mag_EqRhs*             eqOfMIn,
                             G4double                 minimumRadiusOfCurvatureIn):
  BDSIntegratorDipole2(eqOfMIn, minimumRadiusOfCurvatureIn),
  polefaceAngle((*strengthIn)["polefaceangle"]),
  fringeCorr((*strengthIn)["fringecorr"]),
  brho(brhoIn),
  strength(strengthIn)
{
  if (thinElementLength < 0)
    {thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();}
}

void BDSIntegratorDipoleFringe::Stepper(const G4double yIn[],
                                        const G4double dydx[],
                                        const G4double h,
                                        G4double       yOut[],
                                        G4double       yErr[])
{
  G4double yTemp[7];
  // do the dipole kick using base class
  BDSIntegratorDipole2::Stepper(yIn, dydx, h, yTemp, yErr);

  // don't do fringe kick if we're sampling the field  for a long step
  if (h > 1*CLHEP::cm)
    {return;}

  G4ThreeVector pos = G4ThreeVector(yTemp[0], yTemp[1], yTemp[2]);
  G4ThreeVector mom = G4ThreeVector(yTemp[3], yTemp[4], yTemp[5]);

  BDSStep      localPosMom  = ConvertToLocal(pos, mom, h, true, thinElementLength);
  G4ThreeVector localPos  = localPosMom.PreStepPoint();
  G4ThreeVector localMom  = localPosMom.PostStepPoint();
  G4ThreeVector localMomU = localMom.unit();

  // check for forward going paraxial particles - only
  if (localMomU.z() < 0.9)
    {return;}

  // calculate new position
  G4ThreeVector localCLPosOut;
  G4ThreeVector localCLMomOut;
  OneStep(localPos, localMom, localMomU, h, localCLPosOut, localCLMomOut);

  // convert to global coordinates for output
  BDSStep globalOut = BDSAuxiliaryNavigator::CurvilinearToGlobal(strength, localCLPosOut, localCLMomOut, false, eqOfM->FCof());
  G4ThreeVector globalMom = ConvertAxisToGlobal(localCLMomOut, true);
  G4ThreeVector globalPosOut = globalOut.PreStepPoint();
  G4ThreeVector globalMomOut = globalOut.PostStepPoint();

  // write out values and errors
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]     = pos[i];
      yOut[i + 3] = globalMom[i];
      yErr[i]     = 0;
      yErr[i + 3] = 0;
    }
}

void BDSIntegratorDipoleFringe::OneStep(G4ThreeVector  posIn,
                                        G4ThreeVector  momIn,
                                        G4ThreeVector  momUIn,
                                        G4double       h,
                                        G4ThreeVector& posOut,
                                        G4ThreeVector& momOut) const
{
  //G4double momMag = momIn.mag();
  //G4double ratio = (eqOfM->FCof() * brho ) / momMag;
  G4double ratio = 1;

  // fraction of kick applied in case of multiple steps in element
  G4double fraction = h / thinElementLength;

  // prevent overkicking
  if (fraction > 1)
    {fraction = 1;}

  // nominal bending radius.
   G4double momInMag = momIn.mag();

  // nominal bending radius.
  G4double magnetRho = brho / (*strength)["field"];

  G4double x0  = posIn.x() / CLHEP::m;
  G4double y0  = posIn.y() / CLHEP::m;
  G4double s0  = posIn.z();
  G4double xp  = momUIn.x();
  G4double yp  = momUIn.y();
  G4double zp  = momUIn.z();

  G4double x1  = x0;
  G4double y1  = y0;
  G4double s1  = s0;
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  G4double X11=0,X12=0,X21=0,X22 = 0;
  G4double Y11=0,Y12=0,Y21=0,Y22 = 0;

  // calculate fractional fringe field kick
  X11 = 1;
  X21 = ratio * fraction * tan(polefaceAngle) / (magnetRho / CLHEP::m);
  X22 = 1;

  Y11 = 1;
  Y21 = ratio * fraction * -tan(polefaceAngle - fringeCorr) / (magnetRho / CLHEP::m);
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
  momOut = momOutUnit * momInMag;

  posOut = G4ThreeVector(x1, y1, s1);
}