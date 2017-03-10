#include "BDSFieldMagGradient.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "BDSInterpolatorType.hh"
#include "BDSFieldMag.hh"
#include "BDSMagnetStrength.hh"
#include "G4String.hh"
#include "G4Transform3D.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldMagSkew.hh"

BDSFieldMagGradient::BDSFieldMagGradient(){
//empty constructor
}

G4double BDSFieldMagGradient::GetBy(BDSFieldMag* BField, G4double l, G4double h)
{
    G4double B;
    G4ThreeVector position(l, h, 0);
    G4ThreeVector FieldAtX = BField->GetField(position);
    B =FieldAtX[1]/CLHEP::tesla; //put the B back into units of Tesla
    return B;
}

//One Method to call them all, and in the main thread return them.
BDSMagnetStrength* BDSFieldMagGradient::CalculateMultipoles(BDSFieldMag* BField, G4int order, G4double Brho)
{
    G4cout << "running field gradient calculations" << G4endl;
    BDSMagnetStrength* outputstrengths = new BDSMagnetStrength();
    G4double h =2.5; //distance apart in CLHEP distance units (mm) to place query points.
    G4double rotation[5] = {CLHEP::pi/4, CLHEP::pi/6, CLHEP::pi/8, CLHEP::pi/10, CLHEP::pi/12}; //angles to skew the skew field by, depending on order
    BDSFieldMagSkew* BFieldSkew = new BDSFieldMagSkew::BDSFieldMagSkew(BField,rotation[order-1]); //create a skewed field

        G4double k1 = 1/Brho * FirstDerivative(BField,0,h);
        G4double k1s =1/Brho * FirstDerivative(BFieldSkew,0,h);
        G4cout << "k1:" << k1 << "k1s:" << k1s << G4endl;
        (*outputstrengths)["k1"]=k1;
        (*outputstrengths)["k1s"]=k1s;

        G4double k2 =1/Brho * SecondDerivative(BField,0,h);
        G4double k2s =1/Brho * SecondDerivative(BFieldSkew,0,h);
        G4cout << "k2:" << k2 << "k3s:" << k2s << G4endl;
        (*outputstrengths)["k2"]=k2;
        (*outputstrengths)["k2s"]=k2s;

        G4double k3 =1/Brho * ThirdDerivative(BField,0,h);
        G4double k3s =1/Brho * ThirdDerivative(BFieldSkew,0,h);
        G4cout << "k3:" << k3 << "k3s:" << k3s << G4endl;
        (*outputstrengths)["k3"]=k3;
        (*outputstrengths)["k3s"]=k3s;

        G4double k4 =1/Brho * FourthDerivative(BField,0,h);
        G4double k4s =1/Brho * FourthDerivative(BFieldSkew,0,h);
        G4cout << "k4:" << k4 << "k4s:" << k4s << G4endl;
        (*outputstrengths)["k4"]=k4;
        (*outputstrengths)["k4s"]=k4s;

        G4double k5 =1/Brho * FifthDerivative(BField,0,h);
        G4double k5s =1/Brho * FifthDerivative(BFieldSkew,0,h);
        G4cout << "k5:" << k5 << "k5s:" << k5s << G4endl;
        (*outputstrengths)["k5"]=k5;
        (*outputstrengths)["k5s"]=k5s;

    return outputstrengths;
}

//Indvidual Functions to find derivatives

G4double BDSFieldMagGradient::FirstDerivative(BDSFieldMag* BField, G4double x, G4double h)
{
    G4double firstorder=(-GetBy(BField,(x+2*h)) + 8*GetBy(BField,(x+h)) - 8*GetBy(BField,(x-h)) + GetBy(BField,(x-2*h)))/(12*(h/CLHEP::meter));
    return firstorder;
}

G4double BDSFieldMagGradient::SecondDerivative(BDSFieldMag* BField, G4double x, G4double h)
{
    G4double secondorder=(-FirstDerivative(BField,(x+2*h),h) + 8*FirstDerivative(BField,(x+h),h) - 8*FirstDerivative(BField,(x-h),h) + FirstDerivative(BField,(x-2*h),h))/(12*h);
    return secondorder;
}

G4double BDSFieldMagGradient::ThirdDerivative(BDSFieldMag* BField, G4double x, G4double h)
{
    G4double thirdorder=(-SecondDerivative(BField,(x+2*h),h) + 8*SecondDerivative(BField,(x+h),h) - 8*SecondDerivative(BField,(x-h),h) + SecondDerivative(BField,(x-2*h),h))/(12*h);
    return thirdorder;
}

G4double BDSFieldMagGradient::FourthDerivative(BDSFieldMag* BField, G4double x, G4double h)
{
    G4double fourthorder=(-ThirdDerivative(BField,(x+2*h),h) + 8*ThirdDerivative(BField,(x+h),h) - 8*ThirdDerivative(BField,(x-h),h) + ThirdDerivative(BField,(x-2*h),h))/(12*h);
    return fourthorder;
}

//last order
G4double BDSFieldMagGradient::FifthDerivative(BDSFieldMag* BField, G4double x, G4double h)
{
    G4double fifthorder=(-FourthDerivative(BField,(x+2*h),h) + 8*FourthDerivative(BField,(x+h),h) - 8*FourthDerivative(BField,(x-h),h) + FourthDerivative(BField,(x-2*h),h))/(12*h);
    return fifthorder;
}