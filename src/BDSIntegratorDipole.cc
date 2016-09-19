#include "BDSDebug.hh"
#include "BDSIntegratorDipole.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <utility>


BDSIntegratorDipole::BDSIntegratorDipole(BDSMagnetStrength const*  strength,
					 G4double                  brho,
					 G4Mag_EqRhs*              eqOfMIn):
  BDSIntegratorBase(eqOfMIn, 6),
  angle(-(*strength)["angle"]),
  length((*strength)["length"]),
  bField((*strength)["field"]),
  brho(brho)
{
  bPrime = brho * (*strength)["k1"];
  nominalMom = BDSGlobalConstants::Instance()->BeamMomentum();
  cOverGeV = BDSGlobalConstants::Instance()->COverGeV();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B Field " << bField << G4endl;
  G4cout << __METHOD_NAME__ << "B'      " << bPrime << G4endl;
#endif
}

void BDSIntegratorDipole::AdvanceHelix(const G4double  yIn[],
				       const G4double dydx[],
				       G4ThreeVector  /*bField*/,
				       G4double  h,
				       G4double yOut[],
				       G4double yErr[])
{
  // UNCOMMENT TO USE ONLY the g4 stepper for testing
  // use a classical Runge Kutta stepper here
  //backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
  //return;
  G4ThreeVector GlobalPosition = G4ThreeVector(yIn[0], yIn[1], yIn[2]);  
  
#ifdef BDSDEBUG
  G4double charge = (eqOfM->FCof())/CLHEP::c_light;
  G4cout << "BDSIntegratorDipole: step= " << h/CLHEP::m << " m" << G4endl
         << " x  = " << yIn[0]/CLHEP::m     << " m" << G4endl
         << " y  = " << yIn[1]/CLHEP::m     << " m" << G4endl
         << " z  = " << yIn[2]/CLHEP::m     << " m" << G4endl
         << " px = " << yIn[3]/CLHEP::GeV   << " GeV/c" << G4endl
         << " py = " << yIn[4]/CLHEP::GeV   << " GeV/c" << G4endl
         << " pz = " << yIn[5]/CLHEP::GeV   << " GeV/c" << G4endl
         << " q  = " << charge/CLHEP::eplus << " e" << G4endl
	 << " B  = " << bField/(CLHEP::tesla) << " T" << G4endl
    //         << " k= " << kappa/(1./CLHEP::m2) << "m^-2" << G4endl
         << G4endl; 
#endif

  const G4double *pIn = yIn+3;
  G4ThreeVector v0    = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double      InitMag        = v0.mag();
  G4ThreeVector InitMomDir     = v0.unit();
  G4double rho = length/std::abs(angle);
  //G4double rho = InitMag/CLHEP::GeV/(cOverGeV * bField/CLHEP::tesla * charge) * CLHEP::m;

  // in case of zero field (though what if there is a quadrupole part..)
  // or neutral particles do a linear step:
  if(bField==0 || eqOfM->FCof()==0)
    {
      G4ThreeVector positionMove = h * InitMomDir;
      
      yOut[0] = yIn[0] + positionMove.x();
      yOut[1] = yIn[1] + positionMove.y();
      yOut[2] = yIn[2] + positionMove.z();
      
      yOut[3] = v0.x();
      yOut[4] = v0.y();
      yOut[5] = v0.z();
      
      distChord = 0;
      return;
    }

  // global to local
  BDSStep        localPosMom = ConvertToLocal(GlobalPosition, v0, h, false);
  G4ThreeVector      LocalR  = localPosMom.PreStepPoint();
  G4ThreeVector      Localv0 = localPosMom.PostStepPoint();
  G4ThreeVector      LocalRp = Localv0.unit();
  G4ThreeVector itsInitialR  = LocalR;
  G4ThreeVector itsInitialRp = LocalRp;

  G4double lengthCovered = 0;
  G4double angleCoveredSoFar = 0;

  // calculate length and angle already completed by the stepper
  if (LocalR.z() < 0)
    {lengthCovered = std::abs(length)/2.0 - std::abs(LocalR.z());}
  else
    {lengthCovered = std::abs(length)/2.0 + std::abs(LocalR.z());}
  angleCoveredSoFar = lengthCovered/rho;

  // cludge to counter the start position not being equal to -length/2.
  // This arises from the container length being equal to the chord length,
  // not the element length, therefore a small fraction of the dipole does
  // not bend the particles trajectory accordingly.
  G4bool needToUpdateAngle=false;
  G4double target = 0.01*length;
  G4double temp = length/2.0 - std::abs(LocalR.z());
  if ((temp < target) && (LocalR.z() < 0))
    {needToUpdateAngle=true;}

  // update the particle direction according to cludge.
  if (needToUpdateAngle)
    {LocalRp.rotateY(-angleCoveredSoFar);}

  // temporary copy of the rotated momentum for debugging purposes.
  G4ThreeVector LocalRp2 = LocalRp;

  // relative mom. spread
  momSpread = (nominalMom - InitMag)/nominalMom;

  G4double halftheta = h/(rho*2.0);
  G4double halfangle = angle/2.0;

  G4double angleCoveredByThisStep = 2.0*halftheta;
  G4double angleToRotateByToGetToChord = 0;

  // calculate rotation angle to point along the chord
  if (angleCoveredSoFar < halfangle)
    {angleToRotateByToGetToChord = halfangle - angleCoveredSoFar;}
  else if (angleCoveredSoFar > halfangle)
    {angleToRotateByToGetToChord = angleCoveredSoFar - halfangle;}

  // rotate momentum so as to point the central trajectory
  // along the chord of the dipole, and renormalize to unity.
  if (LocalR.z()<0)
    {LocalRp.rotateY(-angleToRotateByToGetToChord);}
  else
    {LocalRp.rotateY(angleToRotateByToGetToChord);}
  LocalRp = LocalRp.unit();

  // advance the orbit
  std::pair<G4ThreeVector,G4ThreeVector> RandRp = updatePandR(rho,h,LocalR,LocalRp);
  G4ThreeVector itsFinalPoint = RandRp.first;
  G4ThreeVector itsFinalDir = RandRp.second;

  // rotate the momentum back to the original direction at the start, then rotate by the
  // angle of the dipole that would've been covered in this step
  if (LocalR.z() > 0)
    {itsFinalDir.rotateY(-angleToRotateByToGetToChord);}
  else
    {itsFinalDir.rotateY(angleToRotateByToGetToChord);}
  itsFinalDir.rotateY(-angleCoveredByThisStep);

  G4double CosT_ov_2=cos(h/rho/2.0);
  distChord = fabs(rho)*(1.-CosT_ov_2);

  // check for paraxial approximation:
  if(LocalRp.z() > 0.9)
  {
	BDSStep globalPosDir = ConvertToGlobalStep(itsFinalPoint, itsFinalDir, false);
	G4ThreeVector GlobalPositionOut = globalPosDir.PreStepPoint();
	G4ThreeVector GlobalTangent  = globalPosDir.PostStepPoint();	
	GlobalTangent*=InitMag; // multiply the unit direction by magnitude to get momentum

	yOut[0] = GlobalPositionOut.x();
	yOut[1] = GlobalPositionOut.y();
	yOut[2] = GlobalPositionOut.z();
	
	yOut[3] = GlobalTangent.x();
	yOut[4] = GlobalTangent.y();
	yOut[5] = GlobalTangent.z();
	return;
  }
  else
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " local helical steps - using G4ClassicalRK4" << G4endl;
#endif
      // use a classical Runge Kutta stepper here
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
    }
}

void BDSIntegratorDipole::Stepper(const G4double yInput[],
				  const G4double dydx[],
				  const G4double hstep,
				  G4double yOut[],
				  G4double yErr[])
{
  G4double err = 1e-10 * hstep; // very small linear increase with distance
  for(G4int i=0; i<nVariables; i++)
    {yErr[i] = err;}

  AdvanceHelix(yInput,dydx,(G4ThreeVector)0,hstep,yOut,yErr);
}

std::pair<G4ThreeVector,G4ThreeVector> BDSIntegratorDipole::updatePandR(G4double rho,
                                            G4double h,
                                            G4ThreeVector LocalR,
                                            G4ThreeVector LocalRp) {
    // see the developer manual for details on the matrix elements and
    // conditional statements.

    // temporary copy of the input position and momentum for debugging purposes
    G4ThreeVector RCopy = LocalR;
    G4ThreeVector RpCopy = LocalRp;

    // quad, dipole, and combined magnet strengths
    G4double K1 = bPrime / brho;
    G4double k0 = 1.0 / pow(rho,2);
    G4double K = K1 + k0;//pow(k0, 2);
    G4double rootK = sqrt(std::abs(K));
    G4double theta = h * rootK;

    G4double rootKy = rootK;
    G4double thetaY = theta;


    G4double n = bPrime / brho;// * pow(rho, 2);
    //G4double kx2 = (1 - n) / pow(rho, 2);
    G4double kx2 = n + k0;// / pow(rho, 2);
    G4double rootKx = sqrt(std::abs(kx2));
    G4double thetaX = h * rootKx;

    G4double xCosTerm, yCosTerm, xSinTerm, ySinTerm;

    // sign for matrix terms which depend on the sign of K1
    G4double sign  = (std::signbit(K1)) ? (-1.0) : (1.0);

    if (n > 0)
      {
        xCosTerm = cos(thetaX);
        yCosTerm = cosh(thetaY);
        xSinTerm = sin(thetaX);
        ySinTerm = sinh(thetaY);
      }
    else
      {
        xCosTerm = cosh(thetaX);
        yCosTerm = cos(thetaY);
        xSinTerm = sinh(thetaX);
        ySinTerm = sin(thetaY);
      }

    // matrix terms
    G4double X11 = xCosTerm;
    G4double X12 = (1.0/rootKx) * xSinTerm;
    G4double X21 = -sign * rootKx * xSinTerm ;
    G4double X22 = X11;

    G4double X16 = -sign * (1.0/rootKx) * (1-xCosTerm);
    G4double X26 = -sign * xSinTerm;

    G4double Y11 = yCosTerm;
    G4double Y12 = (1.0/rootKy) * ySinTerm;
    G4double Y21 = 1.0*sign* rootKy * ySinTerm;
    G4double Y22 = Y11;

    G4double x1, y1, z1, xp1, yp1, zp1;
    x1  = LocalR.x()*X11 + LocalRp.x()*X12;
    y1  = LocalR.y()*Y11 + LocalRp.y()*Y12;
    z1  = LocalR.z()     + h;
    xp1 = LocalR.x()*X21 + LocalRp.x()*X22;
    yp1 = LocalR.y()*Y21 + LocalRp.y()*Y22;

    if (K1 < 0){
      x1  -= X16*momSpread;
      xp1 -= X26*momSpread;
    }
    else if (K1 > 0){
        x1  += X16*momSpread;
        xp1 += X26*momSpread;
    }
    else{
        x1  += X16*momSpread;
        xp1 -= X26*momSpread;
    }
    // calculate zp1 separately to ensure momentum conservation.
    zp1 = sqrt(1.0 - (pow(xp1,2) + pow(yp1,2)));

    G4ThreeVector itsFinalPoint   = G4ThreeVector(x1,y1,z1);
    G4ThreeVector itsFinalDir     = G4ThreeVector(xp1,yp1,zp1);

  return std::make_pair(itsFinalPoint,itsFinalDir);

}
