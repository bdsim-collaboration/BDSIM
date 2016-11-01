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
      nfact.push_back(Factorial(i+1));
    }
}

void BDSIntegratorMultipoleThin::AdvanceHelix(const G4double yIn[],
				     const G4double[] /*dxdy*/,
				     const G4double h,
				     G4double       yOut[])
{
  const G4double *pIn    = yIn+3;
  G4ThreeVector GlobalR  = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector GlobalP  = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double      InitMag = GlobalP.mag();

  //Factor for normalising to particle momentum
  G4double normFactor = eqOfM->FCof()/InitMag;
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

  // global to local
  BDSStep   localPosMom = ConvertToLocal(GlobalR, GlobalP, h, false);
  G4ThreeVector LocalR  = localPosMom.PreStepPoint();
  G4ThreeVector Localv0 = localPosMom.PostStepPoint();
  G4ThreeVector LocalRp = Localv0.unit();

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
  G4double momx;
  G4double momy;

  //G4double speedoflight = CLHEP::c_light / (CLHEP::m / CLHEP::second);
  G4double dipoleTerm = b0l*normFactor *zp / vOverc;
  G4int n = 1;
  std::list<double>::iterator kn = bnl.begin();

  //Sum higher order components into one kick
  for (; kn != bnl.end(); n++, kn++)
    {
      momx = 0; //reset to zero
      momy = 0;
      knReal = (*kn) * pow(position,n).real() / nfact[n];
      knImag = (*kn) * pow(position,n).imag() / nfact[n];
      if (!std::isnan(knReal)){
        momx = knReal;
      }
      if (!std::isnan(knImag)) {
        momy = knImag;
      }
      result = {momx,momy};
      kick += result;
    }

  //apply kick

  xp1 -= (kick.real() + dipoleTerm);
  yp1 += kick.imag();

  //Reset n for skewed kicks.
  n=0;
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

  BDSStep globalPosDir = ConvertToGlobalStep(LocalR, LocalRp, false);
  GlobalR = globalPosDir.PreStepPoint();
  GlobalP = globalPosDir.PostStepPoint();	
  // TBC - this normalisation seemed to be missing originally - is this correct?
  GlobalP*=InitMag; // multiply the unit direction by magnitude to get momentum

  yOut[0] = GlobalR.x();
  yOut[1] = GlobalR.y();
  yOut[2] = GlobalR.z();

  yOut[3] = GlobalP.x();
  yOut[4] = GlobalP.y();
  yOut[5] = GlobalP.z();

  //for(G4int i = 0; i < nVariables; i++)
  //  {yErr[i] = 0;}
}

void BDSIntegratorMultipoleThin::Stepper(const G4double yInput[],
                                     const G4double[] /*dydx[]*/,
                                     const G4double hstep,
                                     G4double yOut[],
                                     G4double yErr[]) {
  G4double yTemp[7];
  G4double h = hstep * 0.5;

  AdvanceHelix(yInput, 0, h, yTemp);
  AdvanceHelix(yTemp, 0, h, yOut);

  h = hstep;
  AdvanceHelix(yInput, 0, h, yTemp);

  for (G4int i = 0; i < nVariables; i++) {
    yErr[i] = yOut[i] - yTemp[i];
    // if error small, set error to 0
    // this is done to prevent Geant4 going to smaller and smaller steps
    // ideally use some of the global constants instead of hardcoding here
    // could look at step size as well instead.
    if (std::abs(yErr[i]) < 1e-7) { yErr[i] = 0; }
  }
}

G4int BDSIntegratorMultipoleThin::Factorial(G4int n)
{
  G4int result = 1;
  for (G4int i = 1; i <= n; i++)
    {result *= i;}
  return result;
}
