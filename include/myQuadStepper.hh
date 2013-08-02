/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
*/

#ifndef myQUADSTEPPER_HH
#define myQUADSTEPPER_HH
#include "globals.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"

class myQuadStepper : public G4MagIntegratorStepper
{

public:  // with description
  
  myQuadStepper(G4Mag_EqRhs *EqRhs);
  
  ~myQuadStepper();
  
  virtual void Stepper( const G4double y[],
			const G4double dydx[],
			const G4double h,
			G4double yout[],
			G4double yerr[]  );
  
  virtual G4double DistChord()   const;
  // Estimate maximum distance of curved solution and chord ... 
  
  void SetBGrad(G4double aBfield);
  void SetBField(G4double aBfield);
  void SetLength(G4double aLength);
  void SetAngle(G4double aAngle);
  G4double GetBGrad();
  
  void StepperName();
  
public: // without description
  
  virtual G4int IntegratorOrder()const { return 2; }
  G4double itsLength;
  G4double itsAngle;
protected:
  //  --- Methods used to implement all the derived classes -----
  
  void AdvanceHelix( const G4double  yIn[],
		     G4ThreeVector Bfld,
		     G4double  h,
		     G4double  yQuad[]);    // output 
  // A first order Step along a quad inside the field.
  
private:
  
  G4ThreeVector yInitial, yMidPoint, yFinal;
  // Data stored in order to find the chord.
  
  G4Mag_EqRhs*  fPtrMagEqOfMot;
  
  G4double itsBGrad;
  G4double itsBField;
 
  G4double itsDist;
  
};

inline  void myQuadStepper::SetBGrad(G4double aBGrad)
{
  itsBGrad=aBGrad;
}

inline  void myQuadStepper::SetBField(G4double aBField)
{
  itsBField=aBField;
}
inline  void myQuadStepper::SetLength(G4double aLength)
{
  itsLength=aLength;
}
inline  void myQuadStepper::SetAngle(G4double aAngle)
{
  itsAngle=aAngle;
}

inline G4double myQuadStepper::GetBGrad()
{
  return itsBGrad;
}

inline void myQuadStepper::StepperName()
{G4cout<<"myQuadStepper"<<G4endl;}

#endif 
