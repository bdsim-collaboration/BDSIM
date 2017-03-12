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

#include <string>
#include <vector>

BDSFieldMagGradient::BDSFieldMagGradient()
{;}

G4double BDSFieldMagGradient::GetBy(BDSFieldMag* BField, G4double l, G4double h) const
{
    G4double B;
    G4ThreeVector position(l, h, 0);
    G4ThreeVector FieldAtX = BField->GetField(position);
    B =FieldAtX[1]/CLHEP::tesla; //put the B back into units of Tesla
    return B;
}

BDSMagnetStrength* BDSFieldMagGradient::CalculateMultipoles(BDSFieldMag* BField,
							    G4int        order,
							    G4double     Brho)
{
    G4cout << "running field gradient calculations" << G4endl;
    BDSMagnetStrength* outputstrengths = new BDSMagnetStrength();
    G4double h =2.5; //distance apart in CLHEP distance units (mm) to place query points.
    G4double rotation[5] = {CLHEP::pi/4, CLHEP::pi/6, CLHEP::pi/8, CLHEP::pi/10, CLHEP::pi/12}; //angles to skew the skew field by, depending on order

    typedef G4double(BDSFieldMagGradient::*Deriv)(BDSFieldMag*, G4double, G4double);
    std::vector<Deriv> derivativeFunctions = {
      &BDSFieldMagGradient::FirstDerivative,
      &BDSFieldMagGradient::SecondDerivative,
      &BDSFieldMagGradient::ThirdDerivative,
      &BDSFieldMagGradient::FourthDerivative,
      &BDSFieldMagGradient::FifthDerivative
    };

    G4double centreX = 0;
    for (G4int i = 0; i < order; ++i)
      {
	G4double brhoinv = 1./Brho;
	G4double result  = (this->*(derivativeFunctions[i]))(BField, centreX, h);
	(*outputstrengths)["k"+std::to_string(i+1)] = result*=brhoinv;
	
	BDSFieldMagSkew* skewField = new BDSFieldMagSkew(BField, rotation[i]);
	G4double skewResult = (this->*(derivativeFunctions[i]))(skewField, centreX, h);
	(*outputstrengths)["k"+std::to_string(i+1)+"s"] = skewResult *= brhoinv;
	delete skewField;
      }


    G4int centreIndex = 0;
    std::vector<G4double> d = PrepareValues(BField, 5, 0, h, centreIndex);

    // o+1 as we start from k1 upwards - ie, 0th order isn't calculated
    for (G4int o = 0; o < order; ++o)
      {(*outputstrengths)["k" + std::to_string(o+1)] = Derivative(d, o+1, centreIndex, h);}
    
    return outputstrengths;
}

std::vector<G4double> BDSFieldMagGradient::PrepareValues(BDSFieldMag* field,
							 G4int        order,
							 G4double     centreX,
							 G4double     h,
							 G4int&       centreIndex) const
{

  G4int maxN = 2*order + 1;
  centreIndex = maxN; // write out maxN to centre index
  std::vector<G4double> data(2*maxN+1); // must initialise vector as not using push_back
  
  for (G4int i = -maxN; i < maxN; i++)
    {data[maxN + i] = GetBy(field, centreX+(G4double)i*h);}
  return data;
}

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

G4double BDSFieldMagGradient::FifthDerivative(BDSFieldMag* BField, G4double x, G4double h)
{
    G4double fifthorder=(-FourthDerivative(BField,(x+2*h),h) + 8*FourthDerivative(BField,(x+h),h) - 8*FourthDerivative(BField,(x-h),h) + FourthDerivative(BField,(x-2*h),h))/(12*h);
    return fifthorder;
}

G4double BDSFieldMagGradient::Derivative(const std::vector<G4double>& data,
					 const G4int                  order,
					 const G4int                  startIndex,
					 const G4double               h) const
{
  if (order == 0)
    {return data.at(startIndex);}

#if 1
  G4cout << "derivative, order: " << order << G4endl;
#endif

  
  G4int subOrder = order-1;
  G4double result = -Derivative(data, subOrder,startIndex+2,h)
    + 8*Derivative(data, subOrder,startIndex+1, h)
    - 8*Derivative(data, subOrder,startIndex-1, h)
    + Derivative(data, subOrder,startIndex-2, h);

  result /= 12*h;
  return result;
}
