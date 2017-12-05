#ifndef BDSINTEGRATORDIPOLEQUADRUPOLE_H
#define BDSINTEGRATORDIPOLEQUADRUPOLE_H

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class BDSIntegratorDipole2;
class BDSMagnetStrength;
class BDSStep;
class G4Mag_EqRhs;

/**
 * @brief Integrator for combined dipole and quadrupolar field.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorDipoleQuadrupole: public BDSIntegratorMag
{
public:
  BDSIntegratorDipoleQuadrupole(BDSMagnetStrength const* strength,
				G4double                 brhoIn,
				G4Mag_EqRhs*             eqOfMIn,
				G4double minimumRadiusOfCurvatureIn);
  
  virtual ~BDSIntegratorDipoleQuadrupole();

  /// Check if the quadrupole has finite strength and use drift if not. If finite strength,
  /// convert to local curvilinear coordiantes and check for paraxial approximation. If paraxial,
  /// use thick quadrupole matrix for transport, else use the G4ClassicalRK4 backup stepper.
  virtual void Stepper(const G4double y[],
		       const G4double dydx[],
		       const G4double h,
		       G4double       yOut[],
		       G4double       yErr[]);

protected:

  /// Calculate a single step in curvilinear coordinates using dipole quadrupole matrix.
  /// Unit momentum is provided as an argument becuase it is already calculated in the
  /// Stepper method.
  void OneStep(G4ThreeVector  posIn,
	       G4ThreeVector  momIn,
	       G4ThreeVector  momUIn, // assumed unit momentum of momIn
	       G4double       h,
	       G4ThreeVector& posOut,
	       G4ThreeVector& momOut) const;

private:
  /// Private default constructor to enforce use of supplied constructor
  BDSIntegratorDipoleQuadrupole() = delete;

  BDSIntegratorDipole2* dipole;
  G4double              bPrime;
  BDSMagnetStrength const* strength;
};

#endif
