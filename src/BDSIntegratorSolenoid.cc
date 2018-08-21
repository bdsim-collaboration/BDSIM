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
#include "BDSIntegratorSolenoid.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals.hh
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorSolenoid::BDSIntegratorSolenoid(BDSMagnetStrength const* strength,
					     G4double                 brho,
					     G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{
  bField = brho * (*strength)["ks"];
  zeroStrength = !BDS::IsFinite(bField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << bField << G4endl;
#endif
}

void BDSIntegratorSolenoid::Stepper(const G4double yIn[],
				    const G4double dydx[],
				    G4double       h,
				    G4double       yOut[],
				    G4double       yErr[])
{
  // In case of zero field or neutral particles do a linear step
  const G4double fcof = eqOfM->FCof();
  if (zeroStrength || !BDS::IsFinite(fcof))
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector mom     = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      momMag  = mom.mag();
  G4double      kappa   = - 0.5*fcof*bField/momMag;

  // Neutral particle or no strength - advance as a drift.
  if (std::abs(kappa)<1e-20)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector pos     = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector momUnit  = mom.unit();
  G4double      h2       = h*h;
  BDSStep   localPosMom  = ConvertToLocal(pos, mom, h, false);
  G4ThreeVector localPos = localPosMom.PreStepPoint();
  G4ThreeVector localMom = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();
  
  G4double x1=0,xp1=0,y1=0,yp1=0,z1=0,zp1=0; //output coordinates to be
  
  // finite strength - treat as a solenoid
  G4double x0  = localPos.x();
  G4double y0  = localPos.y();
  G4double z0  = localPos.z();
  G4double xp0 = localMomUnit.x();
  G4double yp0 = localMomUnit.y();
  G4double zp0 = localMomUnit.z();
  
  // local r'' (for curvature)
  G4ThreeVector localA;
  localA.setX(-zp0*x0);
  localA.setY( zp0*y0);
  localA.setZ( x0*xp0 - y0*yp0);
  localA *= kappa;
  
  // determine effective curvature 
  G4double localAMag = localA.mag();

  // low strength - drift
  if (localAMag < 1e-15)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }
  
  G4double radiusOfCurvature = std::numeric_limits<double>::max();
  if (BDS::IsFinite(localAMag))
    {radiusOfCurvature = 1./localAMag;} // avoid division by 0
  
  // chord distance (simple quadratic approx)
  G4double dc = h2/(8*radiusOfCurvature);
  if (std::isnan(dc))
    {SetDistChord(0);}
  else
    {SetDistChord(dc);}

  // check for paraxial approximation:
  if(std::abs(zp0) < 0.9)
    {// use a classical Runge Kutta stepper here
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }  

  // RMatrix - from Andy Wolszki's Linear Dynamics lectures (#5, slide 43)
  // http://pcwww.liv.ac.uk/~awolski/main_teaching_Cockcroft_LinearDynamics.htm
  // ( cos^2 (wL)     , (1/2w)sin(2wL)  , (1/2)sin(2wL)  , (1/w)sin^2(wL) ) (x )
  // ( (w/2)sin(2wL)  , cos^2(wL)       ,  -w sin^2(wL)  , (1/2)sin(2wL)  ) (x')
  // ( -(1/2)sin(2wL) , (-1/w)sin^2(wL) , cos^2(wL)      , (1/2w)sin(2wL) ) (y )
  // ( w sin^2(wL)    , (-1/2)sin(2wL)  , (-w/2)sin(2wL) , cos^2(wL)      ) (y')
  
  G4double w       = kappa;
  G4double wL      = kappa*h; 
  G4double cosOL   = std::cos(wL); // w is really omega - so use 'O' to describe - OL = omega*L
  G4double cosSqOL = cosOL*cosOL;
  G4double sinOL   = std::sin(wL);
  G4double sin2OL  = std::sin(2.0*wL);
  G4double sinSqOL = sinOL*sinOL;
  
  // calculate thick lens transfer matrix
  x1  = x0*cosSqOL + (0.5*xp0/w)*sin2OL + (0.5*y0)*sin2OL + (yp0/w)*sinSqOL;
  xp1 = (0.5*x0*w)*sin2OL + xp0*cosSqOL - (w*y0)*sinSqOL + (0.5*yp0)*sin2OL;
  y1  = (-0.5*x0)*sin2OL - (xp0/w)*sinSqOL + y0*cosSqOL + (0.5*yp0/w)*sin2OL;
  yp1 = x0*w*sinSqOL - (0.5*xp0)*sin2OL - (0.5*w*y0)*sin2OL + yp0*cosSqOL;  
  
  z1 = z0 + h;
  // ensure normalisation for vector
  zp1 = std::sqrt(1 - xp1*xp1 - yp1*yp1);
  if (std::isnan(zp1))
    {zp1 = zp0;} 
  
  localPos.setX(x1);
  localPos.setY(y1);
  localPos.setZ(z1);
  localMomUnit.setX(xp1);
  localMomUnit.setY(yp1);
  localMomUnit.setZ(zp1);

  // normalise from unit momentum to absolute momentum
  G4ThreeVector localMomOut = localMomUnit * momMag;

  // convert to global coordinates
  BDSStep globalPosMom = CurvilinearToGlobal(localPos, localMomOut, true);
  G4ThreeVector globalPosOut = globalPosMom.PreStepPoint();
  G4ThreeVector globalMomOut = globalPosMom.PostStepPoint();

  // error along direction of travel really
  G4ThreeVector globalMomOutU = globalMomOut.unit();
  globalMomOutU *= 1e-8;
  
  // write out coordinates and errors to arrays
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]   = globalPosOut[i];
      yOut[i+3] = globalMomOut[i];
      yErr[i]   = globalMomOutU[i]*1e-10; // empirically this has to be very small
      yErr[i+3] = 1e-40;
    }
}
