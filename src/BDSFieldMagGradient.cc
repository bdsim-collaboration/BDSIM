#include "BDSFieldMagGradient.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"


#include "BDSInterpolatorType.hh"
#include "BDSFieldMag.hh"
#include "BDSMagnetStrength.hh"
#include "G4String.hh"
#include "G4Transform3D.hh"
#include "BDSFieldMagInterpolated2D.hh"

BDSFieldMagGradient::BDSFieldMagGradient(){

}

G4double BDSFieldMagGradient::GetBy(BDSFieldMagInterpolated2D* BField, G4double l, G4double angle)
{
    G4double B;
//    if (angle==0)
//    {
        G4ThreeVector position(l, 0, 0);
        G4ThreeVector FieldAtX = BField->GetField(position);
        B =FieldAtX[1];
        G4cout << "X=" << position << " B=" << FieldAtX << G4endl;
//    }
//    else
//    {
//        G4double x=l*cos(angle);
//        G4double y=l*sin(angle);
//        G4ThreeVector position(x, y, 0);
//        G4ThreeVector FieldAtX = BField->GetField(position);
//        B=FieldAtX[0]*sin(angle)+FieldAtX[1]*cos(angle);
//    }
    return B;
}

G4double BDSFieldMagGradient::FirstDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h)
{
    G4double firstorder=(-GetBy(BField,(x+2*h)) + 8*GetBy(BField,(x+h)) - 8*GetBy(BField,(x-h)) + GetBy(BField,(x-2*h)))/(12*h);
    return firstorder;
}

G4double BDSFieldMagGradient::SecondDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h)
{
    G4double secondorder=(-FirstDerivative(BField,(x+2*h),h) + 8*FirstDerivative(BField,(x+h),h) - 8*FirstDerivative(BField,(x-h),h) + FirstDerivative(BField,(x-2*h),h))/(12*h);
    return secondorder;
}

G4double BDSFieldMagGradient::ThirdDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h)
{
    G4double thirdorder=(-SecondDerivative(BField,(x+2*h),h) + 8*SecondDerivative(BField,(x+h),h) - 8*SecondDerivative(BField,(x-h),h) + SecondDerivative(BField,(x-2*h),h))/(12*h);
    return thirdorder;
}

G4double BDSFieldMagGradient::FourthDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h)
{
    G4double fourthorder=(-ThirdDerivative(BField,(x+2*h),h) + 8*ThirdDerivative(BField,(x+h),h) - 8*ThirdDerivative(BField,(x-h),h) + ThirdDerivative(BField,(x-2*h),h))/(12*h);
    return fourthorder;
}

//last order
G4double BDSFieldMagGradient::FifthDerivative(BDSFieldMagInterpolated2D* BField, G4double x, G4double h)
{
    G4double fifthorder=(-FourthDerivative(BField,(x+2*h),h) + 8*FourthDerivative(BField,(x+h),h) - 8*FourthDerivative(BField,(x-h),h) + FourthDerivative(BField,(x-2*h),h))/(12*h);
    return fifthorder;
}


//One Method to call them all, and in the main thread return them.
BDSMagnetStrength* BDSFieldMagGradient::CalculateMultipoles(BDSFieldMagInterpolated2D* BField, G4int order)
{
    G4cout << "running field gradient calculations" << G4endl;
    G4double h =5; //distance apart in CLHEP distance units (mm) to place query points. Further from 5 produces increasingly bad results.
    order = 0; //temporary for testing.
    G4cout << "h=" << h << G4endl;//temporary for testing.

    if(order>0)
    {
        G4double k1 = FirstDerivative(BField,0,h);
        G4cout << "First Derivative at 0:" << k1 << G4endl;

    }
    if(order>1)
    {
        G4double k2 = SecondDerivative(BField,0,h);
        G4cout << "Second Derivative at 0:" << k2 << G4endl;
    }
    if(order>2)
    {
        G4double k3 = ThirdDerivative(BField,0,h);
        G4cout << "Third Derivative at 0:" << k3 << G4endl;
    }
    if(order>3)
    {
        G4double k4 = FourthDerivative(BField,0,h);
        G4cout << "Fourth Derivative at 0:" << k4 << G4endl;
    }
    if(order>4)
    {
        G4double k5 = FifthDerivative(BField,0,h);
        G4cout << "Fifth Derivative at 0:" << k5 << G4endl;
    }

    BDSMagnetStrength* outputstrengths = new BDSMagnetStrength();
    return outputstrengths;

}
