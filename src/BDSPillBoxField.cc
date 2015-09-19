#include "BDSPillBoxField.hh"
//#include "BDSAuxiliaryNavigator.hh"
#include "G4Navigator.hh"
#include "CLHEP/Units/PhysicalConstants.h" //speed of light
//#include "CLHEP/GenericFunctions/Bessel.hh"
#include <cmath>
#include "globals.hh"
#include "G4TransportationManager.hh"

BDSPillBoxField::BDSPillBoxField(G4double eFieldMaxIn,
				 G4double cavityRadius,
				 G4double frequencyIn,
				 G4double phaseIn)
{
  frequency = 2.405*CLHEP::c_light/cavityRadius;
  eFieldMax = eFieldMaxIn;
  //PillBoxNavigator = new G4Navigator();
  frequency = frequencyIn;
  phase = phaseIn;
}

G4bool BDSPillBoxField::DoesFieldChangeEnergy() const
{
    return true;
}
//global to local transfomrations?  learn more about this...
void BDSPillBoxField::GetFieldValue(const G4double Point[4],G4double *Bfield ) const
{
  G4ThreeVector LocalR;  //Vector to hold the local coordinates
  G4ThreeVector GlobalR; //Vector to hold the global coordinates

  //Initializing GlobalR.  (GetFieldValue is given global coordinates)
  GlobalR.setX(Point[0]); 
  GlobalR.setY(Point[1]);
  GlobalR.setZ(Point[2]);

  auxNavigator->LocateGlobalPointAndSetup(GlobalR); //Give navigator global coordinates
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform(); //transform to local
  LocalR = GlobalAffine.TransformPoint(GlobalR);  //initialize LocalR with local coords
  
    //No z-dependence because this is TM010
  //We ignore the perturbation provided by the aperture
  //double theta = atan2(LocalR.y(),LocalR.x()); //Angle between x and y for converting B_theta to cart.
  //double B_theta = -(eFieldMax/CLHEP::c_light)*J_1()*sin(frequency*Point[3]);
  //double B_theta = -(eFieldMax/CLHEP::c_light)*sin(CLHEP::pi*CLHEP::c_light*pow(Point[0]*Point[0]+Point[1]*Point[1],0.5)/(2*cavityRadius))*(sin(frequency*Point[3]));
  //double B_theta = -(eFieldMax/CLHEP::c_light)*sin(CLHEP::pi*CLHEP::c_light*pow(LocalR.x()*LocalR.x()+LocalR.y()*LocalR.y(),0.5)/(2*cavityRadius));//*(sin(frequency*Point[3]));


  //Bfield[0] = B_theta*sin(theta);
  Bfield[0] = 0.0 ;//eFieldMax*cos(Point[3]);
  //Bfield[1] = B_theta*cos(theta);
  Bfield[1] = 0.0;
  Bfield[2] = 0.0; //B Field has no z-component.
  //E_z = eFieldMax*J_0(k_r*r) * cos(frequency t)
  Bfield[3] = 0.0; //EField only along z-axis
  Bfield[4] = 0.0; //Efield only along z-axis
  Bfield[5] = eFieldMax*cos(frequency*Point[3]);
  //Bfield[5] = eFieldMax;
  }

BDSPillBoxField::~BDSPillBoxField()
{
}
