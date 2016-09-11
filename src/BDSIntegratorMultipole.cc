#include "BDSDebug.hh"
#include "BDSIntegratorMultipole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"
#include "BDSGlobalConstants.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <G4TwoVector.hh>

BDSIntegratorMultipole::BDSIntegratorMultipole(BDSMagnetStrength const* strength,
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
      bnl.push_back((*strength)[*nkey] / CLHEP::m);
      bsl.push_back((*strength)[*skey] / CLHEP::m);
      nfact.push_back(Factorial(i));
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

void BDSIntegratorMultipole::Stepper(const G4double yIn[],
				     const G4double[] /*dxdy*/,
				     const G4double h,
				     G4double       yOut[],
				     G4double       yErr[])
{
  const G4double *pIn    = yIn+3;
  G4ThreeVector GlobalR  = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector GlobalP  = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double      InitPMag = GlobalP.mag();

  //Factor for normalising to particle momentum
  G4double normFactor = eqOfM->FCof()/InitPMag;
  // eqOfM->FCof() gives us conversion to MeV,mm and rigidity in Tm correctly
  /*
  // this is excessive for debug - only uncomment if debugging this tracking code
#ifdef BDSDEBUG
  G4double charge = (eqOfM->FCof())/CLHEP::c_light;
  G4cout << "BDSIntegratorQuadrupole: step = " << h/CLHEP::m << " m" << G4endl
         << " x  = " << yIn[0]/CLHEP::m     << " m"     << G4endl
         << " y  = " << yIn[1]/CLHEP::m     << " m"     << G4endl
         << " z  = " << yIn[2]/CLHEP::m     << " m"     << G4endl
         << " px = " << yIn[3]/CLHEP::GeV   << " GeV/c" << G4endl
         << " py = " << yIn[4]/CLHEP::GeV   << " GeV/c" << G4endl
         << " pz = " << yIn[5]/CLHEP::GeV   << " GeV/c" << G4endl
         << " q  = " << charge/CLHEP::eplus << " e"     << G4endl
         << " dBy/dx = " << bPrime/(CLHEP::tesla/CLHEP::m) << " T/m" << G4endl
         << " k = " << k/(1./CLHEP::m2) << " m^-2" << G4endl
         << G4endl;
         #endif
  */

  G4ThreeVector LocalR  = ConvertToLocal(GlobalR);
  G4ThreeVector LocalRp = ConvertAxisToLocal(GlobalR,GlobalP);

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
  G4double vOverc = 1;

  //G4double speedoflight = CLHEP::c_light / (CLHEP::m / CLHEP::second);
  G4double dipoleTerm = b0l*normFactor *zp / vOverc;
  G4int n = 0;
  std::list<double>::iterator kn = bnl.begin();

  //Sum higher order components into one kick
  for (; kn != bnl.end(); n++, kn++)
    {
      knReal = (*kn) * pow(position,n).real() / nfact[n];
      knImag = (*kn) * pow(position,n).imag() / nfact[n];
      result = {knReal,knImag};
      kick += result;
    }

  //apply kick
  if(!std::isnan(kick.real()))
    {xp1 -= (kick.real() + dipoleTerm);}
  if(!std::isnan(kick.imag()))
    {yp1 += kick.imag();}

  //Reset n for skewed kicks.
  n=0;
  G4double ksReal = 0;
  G4double ksImag = 0;
  G4double skewAngle=0;

  G4ThreeVector mom = G4ThreeVector(xp1,yp1,zp1);
  G4double momx = mom.x();
  G4double momy = mom.y();

  G4complex skewresult(0,0);
  G4complex skewkick(0,0);

  std::list<double>::iterator ks = bsl.begin();
  for (; ks != bsl.end(); n++, ks++)
    {
      //Rotate momentum vector about z axis according to number of poles
      //then apply each kick seperately and rotate back
      skewAngle = CLHEP::pi / (2*(n+2));
      mom.rotateZ(skewAngle);
      
      // calculate and apply kick
      ksReal = (*ks) * pow(position,n).real() / nfact[n];
      ksImag = (*ks) * pow(position,n).imag() / nfact[n];
      skewresult = {ksReal,ksImag};
      
      // Rotate back
      if(!std::isnan(skewresult.real()))
	{momx = mom.x() - skewresult.real();}
      if(!std::isnan(skewresult.imag()))
	{momy = mom.y() + skewresult.imag();}

      mom = {momx, momy, mom.z()};
      mom.rotateZ(-skewAngle);
    }

  xp1 = mom.x();
  yp1 = mom.y();

  LocalR.setX(x1);
  LocalR.setY(y1);
  LocalR.setZ(z1);
  
  LocalRp.setX(xp1);
  LocalRp.setY(yp1);
  LocalRp.setZ(zp1);

  GlobalR = ConvertToGlobal(LocalR);
  GlobalP = ConvertAxisToGlobal(LocalRp);

  yOut[0] = GlobalR.x();
  yOut[1] = GlobalR.y();
  yOut[2] = GlobalR.z();

  yOut[3] = GlobalP.x();
  yOut[4] = GlobalP.y();
  yOut[5] = GlobalP.z();

  for(G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
}

G4int BDSIntegratorMultipole::Factorial(G4int n)
{
  G4int result = 1;
  for (G4int i = 1; i <= n; i++)
    {result *= i;}
  return result;
}
