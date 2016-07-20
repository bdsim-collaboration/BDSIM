#include "BDSDebug.hh"
#include "BDSIntegratorMultipole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorMultipole::BDSIntegratorMultipole(BDSMagnetStrength const* strength,
						 G4double                 brho,
						 G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorBase(eqOfMIn, 6),
  yInitial(0), yMidPoint(0), yFinal(0)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  bPrime = - brho * (*strength)["k1"];

  std::vector<G4String> normKeys = strength->NormalComponentKeys();
  std::vector<G4String> skewKeys = strength->SkewComponentKeys();
  std::vector<G4String>::iterator nkey = normKeys.begin();
  std::vector<G4String>::iterator skey = skewKeys.begin();

  for (G4int i = 0; i < normKeys.size(); i++, nkey++, skey++)
    {
        knl.push_back((*strength)[*nkey]);
        ksl.push_back((*strength)[*skey]);
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

void BDSIntegratorMultipole::AdvanceHelix(const G4double yIn[],
					   const G4double dydx[],
					   const G4double h,
					   G4double       yOut[],
					   G4double       yErr[])
{
  const G4double *pIn      = yIn+3;
  G4ThreeVector GlobalR    = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector GlobalP    = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double      InitPMag   = GlobalP.mag();
  // quad strength k normalised to charge and momentum of this particle
  // note bPrime was calculated w.r.t. the nominal rigidity.
  G4double k = eqOfM->FCof()*bPrime/InitPMag;
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
  G4ThreeVector LocalRp = G4ThreeVector(dydx[0], dydx[1], dydx[2]);
  
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

  G4complex deflection(0,0);
  G4complex summ(x0, y0);
  G4complex temp(0,0);
  G4double real = 0;
  G4double imag = 0;

  G4int n = 1;
  std::list<double>::iterator kn = knl.begin();

  for (; kn != knl.end(); n++, kn++)
    {
      real = (*kn) * pow(summ,n).real() / factorial(n);
      imag = (*kn) * pow(summ,n).imag() / factorial(n);
      temp = {real,imag};
      deflection += temp;
    }

  xp1 -= deflection.real();
  yp1 += deflection.imag();

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
}

void BDSIntegratorMultipole::Stepper(const G4double yInput[],
				      const G4double dydx[],
				      const G4double h,
				      G4double       yOut[],
				      G4double       yErr[])
{
  const G4double *pIn   = yInput+3;
  G4ThreeVector GlobalP = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double InitPMag     = GlobalP.mag();
  G4double kappa        = - eqOfM->FCof()*bPrime/InitPMag;
  
  if(std::abs(kappa) < 1e-9) //kappa is small - no error needed for paraxial treatment
    {AdvanceHelix(yInput,dydx,h,yOut,yErr);}
  else
    {
      // Compute errors by making two half steps
      G4double yTemp[6], yIn[6];
      
      // Saving yInput because yInput and yOut can be aliases for same array
      for(G4int i = 0; i < nVariables; i++)
	{yIn[i] = yInput[i];}
      
      // Do two half steps
      G4double hstep = h * 0.5; 
      AdvanceHelix(yIn,   dydx, hstep, yTemp, yErr);
      AdvanceHelix(yTemp, dydx, hstep, yOut,  yErr);
      
      // Do a full Step
      AdvanceHelix(yIn, dydx, h, yTemp, yErr);
      
      for(G4int i = 0; i < nVariables; i++)
	{
	  yErr[i] = yOut[i] - yTemp[i];
	  // if error small, set error to 0
	  // this is done to prevent Geant4 going to smaller and smaller steps
	  // ideally use some of the global constants instead of hardcoding here
	  // could look at step size as well instead.
	  if (std::abs(yErr[i]) < 1e-7)
	    {yErr[i] = 0;}
	}
    }
}

G4int BDSIntegratorMultipole::factorial(G4int n){
  G4int result = 1;
  for (G4int i = 1; i <= n; i++){
    result *= i;}
  return result;
}