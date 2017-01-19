#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorMultipoleThin.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <G4TwoVector.hh>

BDSIntegratorMultipoleThin::BDSIntegratorMultipoleThin(BDSMagnetStrength const* strength,
						       G4double                 brho,
						       G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorBase(eqOfMIn, 6),
  yInitial(0), yMidPoint(0), yFinal(0)
{
  b0l = (*strength)["field"] * brho;
  std::vector<G4String> normKeys = strength->NormalComponentKeys();
  std::vector<G4String> skewKeys = strength->SkewComponentKeys();
  std::vector<G4String>::iterator nkey = normKeys.begin();
  std::vector<G4String>::iterator skey = skewKeys.begin();
  for (G4double i = 0; i < normKeys.size(); i++, nkey++, skey++)
    {
      bnl.push_back((*strength)[*nkey] / pow(CLHEP::m,i+1));
      bsl.push_back((*strength)[*skey] / pow(CLHEP::m,i+1));
      nfact.push_back(Factorial(i));
    }
}

void BDSIntegratorMultipoleThin::AdvanceHelix(const G4double yIn[],
					      const G4double[] /*dxdy*/,
					      const G4double h,
					      G4double       yOut[])
{
  const G4double *pIn      = yIn+3;
  G4ThreeVector GlobalR    = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector GlobalP    = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double      InitMag    = GlobalP.mag();
  
  // global to local
  BDSStep   localPosMom = ConvertToLocal(GlobalR, GlobalP, h, false);
  G4ThreeVector LocalR  = localPosMom.PreStepPoint();
  G4ThreeVector Localv0 = localPosMom.PostStepPoint();
  G4ThreeVector LocalRp = Localv0.unit();

  if (LocalRp.z() < 0.9) //for non paraxial, advance particle as if in a drift
  {
    AdvanceDrift(yIn,GlobalP,h,yOut);
    return;
  }

  G4double x0  = LocalR.x();
  G4double y0  = LocalR.y();
  G4double z0  = LocalR.z();
  G4double xp  = LocalRp.x();
  G4double yp  = LocalRp.y();
  G4double zp  = LocalRp.z();

  // initialise output varibles with input position as default
  G4double x1  = x0;
  G4double y1  = y0;
  G4double z1  = z0 + h; // new z position will be along z by step length h
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  //Kicks come from pg 27 of mad-8 physical methods manual
  G4complex kick(0,0);
  G4complex position(x0, y0);
  G4complex result(0,0);
  //Components of complex vector
  G4double knReal = 0;
  G4double knImag = 0;
  G4double momx;
  G4double momy;

  G4int n = 1;
  std::list<double>::iterator kn = bnl.begin();

  //Sum higher order components into one kick
  for (; kn != bnl.end(); n++, kn++)
    {
      momx = 0; //reset to zero
      momy = 0;
      knReal = (*kn) * pow(position,n).real() / nfact[n];
      knImag = (*kn) * pow(position,n).imag() / nfact[n];
      if (!std::isnan(knReal))
	{momx = knReal;}
      if (!std::isnan(knImag))
	{momy = knImag;}
      result = {momx,momy};
      kick += result;
    }

  //apply kick

  xp1 -= kick.real();
  yp1 += kick.imag();
  zp1 = sqrt(1 - pow(xp1,2) - pow(yp1,2));

  //Reset n for skewed kicks.
  n=1;
  G4double ksReal = 0;
  G4double ksImag = 0;
  G4double skewAngle=0;

  G4ThreeVector mom = G4ThreeVector(xp1,yp1,zp1);
  momx = mom.x();
  momy = mom.y();

  G4complex skewresult(0,0);
  G4complex skewkick(0,0);

  std::list<double>::iterator ks = bsl.begin();
  for (; ks != bsl.end(); n++, ks++)
    {
      if (BDS::IsFinite(*ks))
        {
          //Rotate momentum vector about z axis according to number of poles
          //then apply each kick separately and rotate back
          //skewAngle = CLHEP::pi / (2 * n);
          skewAngle = CLHEP::halfpi;
          mom.rotateZ(skewAngle);

          // calculate and apply kick
          ksReal = (*ks) * pow(position, n).real() / nfact[n];
          ksImag = (*ks) * pow(position, n).imag() / nfact[n];
          skewresult = {ksReal, ksImag};

          // Rotate back
          if (!std::isnan(skewresult.real()))
            {momx = mom.x() - skewresult.real();}
          if (!std::isnan(skewresult.imag()))
            {momy = mom.y() + skewresult.imag();}

          mom = {momx, momy, mom.z()};
          mom.rotateZ(-skewAngle);
        }
    }

  xp1 = mom.x();
  yp1 = mom.y();
  zp1 = sqrt(1 - pow(xp1,2) - pow(yp1,2));

  LocalR.setX(x1);
  LocalR.setY(y1);
  LocalR.setZ(z1);
  
  LocalRp.setX(xp1);
  LocalRp.setY(yp1);
  LocalRp.setZ(zp1);

  ConvertToGlobal(LocalR,LocalRp,InitMag,yOut);
}

void BDSIntegratorMultipoleThin::Stepper(const G4double yInput[],
                                     const G4double[] /*dydx[]*/,
                                     const G4double hstep,
                                     G4double yOut[],
                                     G4double yErr[])
{
  AdvanceHelix(yInput, 0, hstep, yOut);

  // The two half-step method cannot be used as the
  // multipole kick will be applied twice meaning
  // the xp and yp values in the output arrays will be
  // out by a factor of two. This could potentially
  // lead to an incorrectly large error, therefore the
  // error is set to 0 here.

  for (G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
}

G4int BDSIntegratorMultipoleThin::Factorial(G4int n)
{
  G4int result = 1;
  for (G4int i = 1; i <= n; i++)
    {result *= i;}
  return result;
}
