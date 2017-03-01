#ifndef BDSINTEGRATORMULTIPOLETHIN_H
#define BDSINTEGRATORMULTIPOLETHIN_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include <list>

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that ignores the field and uses the analytical solution to a multipole.
 * 
 * @author Will Shields
 */

class BDSIntegratorMultipoleThin: public BDSIntegratorMag
{
public:
  BDSIntegratorMultipoleThin(BDSMagnetStrength const* strength,
			     G4double                 brho,
			     G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorMultipoleThin(){;}

  /// The stepper for integration. The stepsize is fixed, equal to h. The reason for this
  /// is so that intermediate steps can be calculated and therefore the error ascertained
  /// or distance from the chord.  Error calculation is not currently implemented.
  virtual void Stepper(const G4double yIn[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

protected:
  /// Calculate the new particle coordinates for a given step length h.
  void AdvanceHelix(const G4double yIn[],
		    const G4double dydx[],
		    const G4double h,
		    G4double       yOut[]);

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorMultipoleThin();

  /// Calculate the factorial of n.
  G4int Factorial(G4int n);

  /// Dipole component
  G4double b0l;
  /// @{ Higher order components
  std::list<double> bnl;
  std::list<double> bsl;
  std::vector<G4int> nfact;
  /// @}
  
  /// Data stored in order to find the chord.
  G4ThreeVector yInitial, yMidPoint, yFinal;
};

#endif
