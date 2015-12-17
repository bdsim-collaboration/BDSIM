#ifndef BDSDipoleStepper_H
#define BDSDipoleStepper_H

#include "BDSAuxiliaryNavigator.hh"

#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Stepper that calculates trajectory througha uniform magnetic field.
 * 
 * Originally part of BDSIM by many authors.
 */

class BDSDipoleStepper: public G4MagIntegratorStepper, public BDSAuxiliaryNavigator
{
public:
  BDSDipoleStepper(G4Mag_EqRhs *EqRhs);
  ~BDSDipoleStepper();
  
  virtual void Stepper( const G4double y[],
			const G4double dydx[],
			const G4double h,
			G4double yout[],
			G4double yerr[]);
  
  virtual G4double DistChord()   const;
  // Estimate maximum distance of curved solution and chord ... 
  
  void SetBGrad(G4double aBfield);
  void SetBField(G4double aBfield);
  void SetLength(G4double aLength);
  void SetAngle(G4double aAngle);
  G4double GetBGrad() const;
  
  void StepperName();
  
  virtual G4int IntegratorOrder()const { return 2; }
  G4double itsLength;
  G4double itsAngle;
  
protected:
  
  void AdvanceHelix(const G4double yIn[],
		    const G4double dydx[],
		    G4ThreeVector  Bfld,
		          G4double h,
		          G4double yOut[],
		          G4double yErr[]);
  
private:
  G4ThreeVector yInitial, yMidPoint, yFinal;
  // Data stored in order to find the chord.
  
  G4Mag_EqRhs*  fPtrMagEqOfMot;
  
  G4double itsBGrad;
  G4double itsBField;
  G4double itsDist;

  // use for high amplitude particles
  G4MagIntegratorStepper* backupStepper;

  /// Total beam energy
  G4double nominalEnergy; 
};

inline  void BDSDipoleStepper::SetBGrad(G4double aBGrad)
{itsBGrad = aBGrad;}

inline  void BDSDipoleStepper::SetBField(G4double aBField)
{itsBField = aBField;}

inline  void BDSDipoleStepper::SetLength(G4double aLength)
{itsLength = aLength;}

inline  void BDSDipoleStepper::SetAngle(G4double aAngle)
{itsAngle = aAngle;}

inline G4double BDSDipoleStepper::GetBGrad() const
{return itsBGrad;}

inline void BDSDipoleStepper::StepperName()
{G4cout<<"BDSDipoleStepper"<<G4endl;}

#endif 
