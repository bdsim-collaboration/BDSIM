#include "BDSDebug.hh"
#include "BDSIntegratorFringefield.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"


BDSIntegratorFringefield::BDSIntegratorFringefield(BDSMagnetStrength const* strength,
                                                   G4double                 brho,
                                                   G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorDipole(strength, brho, eqOfMIn),
  angle((*strength)["angle"]),
  bField((*strength)["field"])
{
  polefaceAngle = (*strength)["polefaceangle"];
  fringeCorr = (*strength)["fringecorr"];
}

void BDSIntegratorFringefield::AdvanceHelix(const G4double yIn[],
                                            const G4double dydx[],
                                            const G4double h,
                                            G4double       yOut[],
                                            G4double       yErr[])
{
#ifdef BDSDEBUG
  G4cout << "BDSIntegratorDipole: step= " << h/CLHEP::m << " m" << G4endl
         << " x  = " << yIn[0]/CLHEP::m     << " m" << G4endl
         << " y  = " << yIn[1]/CLHEP::m     << " m" << G4endl
         << " z  = " << yIn[2]/CLHEP::m     << " m" << G4endl
         << " px = " << yIn[3]/CLHEP::GeV   << " GeV/c" << G4endl
         << " py = " << yIn[4]/CLHEP::GeV   << " GeV/c" << G4endl
         << " pz = " << yIn[5]/CLHEP::GeV   << " GeV/c" << G4endl
    //<< " q  = " << charge/CLHEP::eplus << " e" << G4endl
	 << " B  = " << bField/(CLHEP::tesla) << " T" << G4endl
    //         << " k= " << kappa/(1./CLHEP::m2) << "m^-2" << G4endl
         << G4endl;
#endif

  const G4double *pIn   = yIn+3;
  G4ThreeVector GlobalP = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4ThreeVector GlobalR = G4ThreeVector(yIn[0], yIn[1], yIn[2]);

  G4double      InitMag    = GlobalP.mag();
  G4ThreeVector InitMomDir = GlobalP.unit();

  if(bField==0 || eqOfM->FCof()==0)
    {
      G4ThreeVector positionMove = h * InitMomDir;

      yOut[0] = yIn[0] + positionMove.x();
      yOut[1] = yIn[1] + positionMove.y();
      yOut[2] = yIn[2] + positionMove.z();

      yOut[3] = GlobalP.x();
      yOut[4] = GlobalP.y();
      yOut[5] = GlobalP.z();

      distChord = 0;
      return;
    }

  // global to local
  BDSStep   localPosMom = ConvertToLocal(GlobalR, GlobalP, h, false);
  G4ThreeVector LocalR  = localPosMom.PreStepPoint();
  G4ThreeVector Localv0 = localPosMom.PostStepPoint();
  G4ThreeVector LocalP = Localv0.unit();

  G4double      charge  = (eqOfM->FCof())/CLHEP::c_light;
  G4double      rho = InitMag/CLHEP::GeV/(0.299792458 * bField/CLHEP::tesla * charge) * CLHEP::m;

  std::pair<G4ThreeVector,G4ThreeVector> RandRp = updatePandR(rho,h,LocalR,LocalP);
  LocalR = RandRp.first;
  LocalP = RandRp.second;
  G4double CosT_ov_2 = cos(h/rho/2.0);
  distChord = fabs(rho)*(1.-CosT_ov_2);

  G4double x0  = LocalR.x();
  G4double y0  = LocalR.y();
  G4double z0  = LocalR.z();
  G4double xp  = LocalP.x();
  G4double yp  = LocalP.y();
  G4double zp  = LocalP.z();

  // initialise output varibles with input position as default
  G4double x1  = x0;
  G4double y1  = y0;
  G4double z1  = z0;  // new z position will be along z by step length h
  G4double xp1 = xp;
  G4double kick =(y0 * tan(polefaceAngle - fringeCorr)/rho);
  G4double yp1 = yp - kick;
  G4double zp1 = zp;

  LocalR.setX(x1);
  LocalR.setY(y1);
  LocalR.setZ(z1);

  LocalP.setX(xp1);
  LocalP.setY(yp1);
  LocalP.setZ(zp1);

  if(true)
    {
      BDSStep globalPosDir = ConvertToGlobalStep(LocalR, LocalP, false);
      GlobalR = globalPosDir.PreStepPoint();
      GlobalP = globalPosDir.PostStepPoint();
      GlobalP*=InitMag; // multiply the unit direction by magnitude to get momentum
      
      yOut[0] = GlobalR.x();
      yOut[1] = GlobalR.y();
      yOut[2] = GlobalR.z();
      
      yOut[3] = GlobalP.x();
      yOut[4] = GlobalP.y();
      yOut[5] = GlobalP.z();
      return;
    }
  else
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " local helical steps - using G4ClassicalRK4" << G4endl;
#endif
      G4cout << "Using Runge Kutta "<< G4endl;
      
      // use a classical Runge Kutta stepper here
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
    }
}

void BDSIntegratorFringefield::Stepper(const G4double yInput[],
                                       const G4double dydx[],
                                       const G4double hstep,
                                       G4double       yOut[],
                                       G4double       yErr[])
{
  G4double err = 1e-10 * hstep; // very small linear increase with distance
  for(G4int i=0; i<nVariables; i++)
    {yErr[i] = err;}

  AdvanceHelix(yInput,dydx,hstep,yOut,yErr);
}
