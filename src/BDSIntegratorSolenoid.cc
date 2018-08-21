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
  
  const G4double *pIn      = yIn+3;
  G4ThreeVector GlobalR    = G4ThreeVector( yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom    = G4ThreeVector( pIn[0], pIn[1], pIn[2]);
  G4ThreeVector momUnit = mom.unit();
  G4double      momMag   = mom.mag();
  G4double      kappa      = - 0.5*fcof*bField/momMag;
  G4double      h2         = h*h;
  
  BDSStep   localPosMom = ConvertToLocal(GlobalR, mom, h, false);
  G4ThreeVector localPos  = localPosMom.PreStepPoint();
  G4ThreeVector localMom = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();
  
  G4double x1,xp1,y1,yp1,z1,zp1; //output coordinates to be
  
  if (std::abs(kappa)<1e-12)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }
  
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
  G4double R_1 = localA.mag();

  if (R_1 < 1e-15)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }
  
  // Save for Synchrotron Radiation calculations
  G4double R=1./R_1;
  
  // chord distance (simple quadratic approx)
  G4double dc = h2/(8*R);
  SetDistChord(dc);

  // check for paraxial approximation:
  if(std::abs(zp0) < 0.9)
    {// use a classical Runge Kutta stepper here
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }  

  // RMatrix - from Andy Wolszki's Linear Dynamics lectures (#5, slide 43)
  // http://pcwww.liv.ac.uk/~awolski/main_teaching_Cockcroft_LinearDynamics.htm
  // note this is actually for one step through the whole solenoid as focussing
  // comes from edge effects - may need to reconsider this in the future...
  // maximum step size is set to full length in BDSSolenoid.cc
  // ( cos^2 (wL)     , (1/2w)sin(2wL)  , (1/2)sin(2wL)  , (1/w)sin^2(wL) ) (x )
  // ( (w/2)sin(2wL)  , cos^2(wL)       ,  -w sin^2(wL)  , (1/2)sin(2wL)  ) (x')
  // ( -(1/2)sin(2wL) , (-1/w)sin^2(wL) , cos^2(wL)      , (1/2w)sin(2wL) ) (y )
  // ( w sin^2(wL)    , (-1/2)sin(2wL)  , (-w/2)sin(2wL) , cos^2(wL)      ) (y')
  
  G4double w       = kappa;
  //need the length along curvilinear s -> project h on z
  G4ThreeVector positionMove = h * momUnit;
  G4double dz      = positionMove.z();
  G4double wL      = kappa*dz; 
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
  
  // ensure normalisation for vector
  zp1 = std::sqrt(1 - xp1*xp1 -yp1*yp1);
  if (std::isnan(zp1))
    {zp1 = zp0;}
  
  // calculate deltas to existing coords
  G4double dx = x1-x0;
  G4double dy = y1-y0;
  
  // check for precision problems
  G4double ScaleFac = (dx*dx+dy*dy+dz*dz)/h2;
#ifdef BDSDEBUG
  G4cout << "Ratio of calculated to proposed step length: " << ScaleFac << G4endl;
#endif
  if(ScaleFac>1.0000001)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " normalising to conserve step length" << G4endl;
#endif
      ScaleFac = std::sqrt(ScaleFac);
      if (std::isnan(ScaleFac))
	{ScaleFac = 1;}
      dx /= ScaleFac;
      dy /= ScaleFac;
      dz /= ScaleFac;
      x1 =  x0+dx;
      y1 =  y0+dy;
    }
  z1 = z0+dz;
  
  //write the final coordinates
  localPos.setX(x1);
  localPos.setY(y1);
  localPos.setZ(z1);
  localMomUnit.setX(xp1);
  localMomUnit.setY(yp1);
  localMomUnit.setZ(zp1);

  ConvertToGlobal(localPos, localMomUnit, yOut, yErr, momMag);
}
