#ifndef BDSBUNCHINTERFACE_H
#define BDSBUNCHINTERFACE_H 

#include "globals.hh"
#include "parser/options.h"
#include "BDSGlobalConstants.hh"

namespace CLHEP {
  class HepRandomEngine;
  class HepSymMatrix;
  class HepVector;
  class RandMultiGauss;
}

class BDSBeamline;

/**
 * @brief The base class for bunch distribution generators.
 *
 * @author Stewart Boogert <Stewart.Boogert@rhul.ac.uk>
 */

class BDSBunchInterface
{ 
protected : 
  ///@{ Centre of distributions
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double S0;
  G4double T0; 
  G4double Xp0; 
  G4double Yp0;
  G4double Zp0;
  G4double sigmaT; 
  G4double sigmaE;
  ///@}
  
  /// whether to ignore z and use s and transform for curvlinear coordinates
  G4bool   useCurvilinear;

  /// Create multidimensional Gaussian random number generator
  /// for Twiss and Gauss, could be moved elsewhere
  /// can change sigma matrix to make non-definite
  CLHEP::RandMultiGauss* CreateMultiGauss(CLHEP::HepRandomEngine & anEngine, const CLHEP::HepVector & mu, CLHEP::HepSymMatrix & sigma);

  G4double CalculateZp(G4double xp, G4double yp, G4double Zp0)const;

  /// Transforms the coordinates from initial coordinates about 0,0,0 to
  /// those in a curvilinear system.  Here, z0 is treated as the intended
  /// S coordinate on input and is modifed to be the global z coordinate.
  void ApplyCurvilinearTransform(G4double& x0, G4double& y0, G4double& z0,
				 G4double& xp, G4double& yp, G4double& zp);

public : 
  BDSBunchInterface();
  virtual ~BDSBunchInterface();
  virtual void SetOptions(GMAD::Options& opt);

  /// Each derived class can override this default method of reference
  /// position. If S0 > 0 or derived class changes member bool 'curvilinear'
  /// z0 will be treated as S and the global z0 be calculated.
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);

  G4double GetX0()     {return X0;}
  G4double GetY0()     {return Y0;}
  G4double GetZ0()     {return Z0;}
  G4double GetT0()     {return T0;}
  G4double GetXp0()    {return Xp0;}
  G4double GetYp0()    {return Yp0;}
  G4double GetZp0()    {return Zp0;}
  G4double GetSigmaE() {return sigmaE;}
  G4double GetSigmaT() {return sigmaT;}

private:
  /// A reference to the fully construct beamline that lazy initiated.
  BDSBeamline* beamline;
};

#endif
