#ifndef BDSINTEGRATORCURVILINEAR_H
#define BDSINTEGRATORCURVILINEAR_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegrator.hh"

#include "globals.hh" // geant4 types / globals

class G4Mag_EqRhs;

/**
 * @brief Provision of local coordinate transforms for integrators.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorCurvilinear: public BDSIntegrator, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorCurvilinear(G4Mag_EqRhs* eqOfMIn,
			   G4int        nVariablesIn);
  
  virtual ~BDSIntegratorCurvilinear(){;}
  
protected:
  /// Convert final local position and direction to global frame
  void ConvertToGlobal(const G4ThreeVector& LocalR,
		       const G4ThreeVector& LocalRp,
		       const G4double InitMag,
		       G4double yOut[]);

private:
  /// Private default constructor to force use of specific constructor
  BDSIntegratorCurvilinear();
};

#endif
