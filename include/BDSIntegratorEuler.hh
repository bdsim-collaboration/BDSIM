#ifndef BDSINTEGRATORSYMPLECTICEULER_H
#define BDSINTEGRATORSYMPLECTICEULER_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;

/**
 * @brief BDSIM 2nd order Euler integration.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorEuler: public BDSIntegratorMag
{
public:
  BDSIntegratorEuler(G4Mag_EqRhs* eqOfMIn);

  virtual ~BDSIntegratorEuler();

  /// Calculate output coordinates.
  virtual void Stepper(const G4double yIn[],     ///< Input coordinates x,y,z,px,py,pz,t
		       const G4double dydx[],    ///< Partial differentials at yInput
		       G4double       stepLength,///< Length of trajectory to calculate
		       G4double       yOut[],    ///< Output array
		       G4double       yErr[]);   ///< Output error array

private:
  /// Private default constructor to force use of provided one.
  BDSIntegratorEuler() = delete;
  
  void SimpleStepper(const G4double yIn[],     ///< Input coordinates x,y,z,px,py,pz,t
		     const G4double dydx[],    ///< Partial differentials at yInput
		     G4double       stepLength,///< Length of trajectory to calculate
		     G4double       yOut[],    ///< Output array
		     G4double       yErr[]);
};

#endif
