#include "BDSPillBoxField.hh"
#include "G4Navigator.hh"
#include "CLHEP/Units/PhysicalConstants.h" 
#include "globals.hh"
#include "G4TransportationManager.hh"
#include <cmath>

BDSPillBoxField::BDSPillBoxField(G4double eFieldMaxIn,
				 G4double cavityRadiusIn,
				 G4double frequencyIn,
				 G4double phaseIn):
  eFieldMax(eFieldMaxIn),cavityRadius(cavityRadiusIn),frequency(frequencyIn),phase(phaseIn)
{
}

G4bool BDSPillBoxField::DoesFieldChangeEnergy() const
{
    return true;
}

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
  
 
  G4RotationMatrix globalToLocalMatrix = auxNavigator->NetRotation();   //Matrix of Global to Local
  G4RotationMatrix localToGlobalMatrix = globalToLocalMatrix.inverse();  //Find the inverse of this transform to be used later
  G4double radialDistance = pow(LocalR.x()*LocalR.x() +  LocalR.y()*LocalR.y(),0.5);

  //Calculating the magnitudes of the Fields:

  
  //J0(2.405*z/cavityRadius) ..  Uses polynomial approximation suitable for J0 arg between 0 and 3 
  double J0r = 0.999999999-2.249999879*pow((2.405*radialDistance/(3*cavityRadius)),2)+1.265623060*pow((2.405*radialDistance/(3*cavityRadius)),4)-0.316394552*pow((2.405*radialDistance/(3*cavityRadius)),6)+0.044460948*pow((2.405*radialDistance/(3*cavityRadius)),8)-0.003954479*pow((2.405*radialDistance/(3*cavityRadius)),10)+0.000212950*pow((2.405*radialDistance/(3*cavityRadius)),12); 

  //J0(2.405*z/cavityRadius) ..  Uses polynomial approximation suitable for J1 arg between 0 and 3 
  double J1r = (2.405*radialDistance/(3*cavityRadius))*(0.500000000-0.562499992*pow((2.405*radialDistance/(3*cavityRadius)),2)+0.210937377*pow((2.405*radialDistance/(3*cavityRadius)),4)-0.039550040*pow((2.405*radialDistance/(3*cavityRadius)),6)+0.004447331*pow((2.405*radialDistance/(3*cavityRadius)),8)-0.000330547*pow((2.405*radialDistance/(3*cavityRadius)),10)+0.000015525*pow((2.405*radialDistance/(3*cavityRadius)),12));

  //|E|
  G4double eMagnitude = eFieldMax*J0r*cos(frequency*Point[3]);
  //|H|
  G4double hMagnitude = (eFieldMax/(pow(CLHEP::mu0/CLHEP::epsilon0,0.5))*J1r*sin(frequency*Point[3]));


  //Angle of the position vector.
  G4double angle = atan2(LocalR.y(),LocalR.x()) + CLHEP::pi*0.5; //Add pi/2 so that the vector is perpendicular to position.

  //normalization factor
  G4double normalization = pow(tan(angle)*tan(angle)+1,0.5);

  //Local B and E fields:
  G4ThreeVector LocalB = G4ThreeVector(tan(angle)*hMagnitude/normalization, //x
				       hMagnitude/normalization,            //y
				       0);                                  //z

  G4ThreeVector LocalE = G4ThreeVector(0,                                   //x
				       0,                                   //y
				       eMagnitude);                         //z

  //Converting back to global coordinates:
  G4ThreeVector GlobalB = localToGlobalMatrix*LocalB; //Multiply the localB field by the local2Global matrix to get back into global coordinates
  G4ThreeVector GlobalE = localToGlobalMatrix*LocalE;
  
 
  //BField x, y, z
  Bfield[0] = GlobalB.x();  
  Bfield[1] = GlobalB.y();
  Bfield[2] = GlobalB.z();
  //EField x, y, z 
  Bfield[3] = GlobalE.x();
  Bfield[4] = GlobalE.y();
  Bfield[5] = GlobalE.z();
  }

BDSPillBoxField::~BDSPillBoxField()
{
}
