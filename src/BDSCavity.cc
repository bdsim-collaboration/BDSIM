#include "BDSCavity.hh"
#include "BDSAcceleratorComponent.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

//Elliptical Cavity:
#include "G4Polycone.hh"

//Pillbox:
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include <cmath>
#include <vector>
BDSCavity::BDSCavity(G4String name, //Any others to add here? 
		     G4String type,
		     G4double length,
		     G4Material* cavityMaterialIn,
		     G4Material* vacuumMaterialIn,
		     G4double equatorRadiusIn,
		     G4double irisRadiusIn,
		     G4double thicknessIn
		     ) : 
  BDSAcceleratorComponent(name, length, 0, type),
  cavityMaterial(cavityMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  equatorRadius(equatorRadiusIn),
  irisRadius(irisRadiusIn),
  thickness(thicknessIn)
{
}

void BDSCavity::PlaceComponents()
{
  G4PVPlacement* vacuumPV = new G4PVPlacement(0, //Rotation
					      G4ThreeVector(0,0,0), //Position
					      vacuumLV,             //Logical Volume to be place
					      name + "_vacuum_pv",  //placement name
					      containerLogicalVolume,          //mother volume
					      false,                //pMany unused
					      0,                    //copy number
					      checkOverlaps         //check overlaps
					      );
  
  RegisterPhysicalVolume(vacuumPV); //BDSGeometryComponent

  G4PVPlacement* cavityPV = new G4PVPlacement(0, //Rotation
					      G4ThreeVector(0,0,0), //Position
					      cavityLV,             //Logical Volume to be place
					      name + "_cavity_pv",  //placement name
					      containerLogicalVolume,          //mother volume
					      false,                //pMany unused
					      0,                    //copy number
					      checkOverlaps         //check overlaps
					      );
  RegisterPhysicalVolume(cavityPV); //BDSGeometryComponent


}


void BDSCavity::BuildContainerLogicalVolume() //This ultimately comes from BDSGeometryComponent.  containerSolid and
//containerLogicalVolume are protected g4solids/lvs of this base class.
{
  G4double outerRadius = equatorRadius + lengthSafety;
  containerSolid = new G4Tubs(name + "_container_solid", //name
			      0.0,                       //innerRadius
			      outerRadius,                //outerRadius
			      chordLength *0.5,           //half length
			      0.0,                        //starting angle
			      2.0*CLHEP::pi);               //spanning angle

  SetExtentX(-outerRadius*0.5,outerRadius*0.5);
  SetExtentY(-outerRadius*0.5,outerRadius*0.5);
  SetExtentZ(-chordLength*0.5,chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					emptyMaterial,
					       name + "_container_lv");

    }

void BDSCavity::BuildEllipticalCavityGeometry(//G4double irisRSemiAxis, //iris ellipse vertical semiaxis.
				     //G4double irisZSemiAxis, //iris ellipse horizontal semiaxis
				     //G4double equatorRSemiAxis, //equator ellipse vertical semiaxis
				     //G4double equatorZSemiAxis, //equator ellipse horizontal semiaxis
				     //G4double tangentAngle, //Angle of line connecting ellipses to the vertical.
				     //Ought to be the common tangent with most negative gradient for reasonable output
				     //G4int noPoints //number of points forming each curved section.  Total points for entire quarter section will be 2*noPoints.
					      )
  {

    //Ellipse Centres (Ellipses defined parametrically)
    //iris Centre:

    //::::::::::::::::::::HARD CODED FOR THE TIME BEING:::::::::::::::::::::::::::::::::::::::::::::::
    G4double irisRSemiAxis = 19 * CLHEP::mm ;  //iris ellipse vertical semiaxis.
    G4double irisZSemiAxis = 12 * CLHEP::mm ; //iris ellipse horizontal semiaxis
    G4double equatorRSemiAxis = 42 * CLHEP::mm ;//equator ellipse vertical semiaxis
    G4double equatorZSemiAxis = 42 * CLHEP::mm ; //equator ellipse horizontal semiaxis
    G4double tangentAngle = 13.3 * CLHEP::deg ;
    G4double irisRadius = 35.0 * CLHEP::mm ;
    G4int noPoints = 24;
    
    G4double zi = chordLength/2;
    G4double ri = irisRadius + irisRSemiAxis;
    
    //Equator Centre:
    G4double ze = 0.0;
    G4double re = equatorRadius - equatorRSemiAxis;

    
    //gradient of line connecting the ellipses
    G4double m = tan(tangentAngle + 0.5*CLHEP::pi);

    std::cout << "m = " << m << std::endl;
    std::cout << "irisRSemiAxis = " << irisRSemiAxis << std::endl;
    std::cout << "irisZSemiAxis = " << irisZSemiAxis << std::endl;
    std::cout << "equatorRSemiAxis = " << equatorRSemiAxis << std::endl;
    std::cout << "equatorZSemiAxis = " << equatorZSemiAxis << std::endl;

    
   
    //Values of parameters such that e.g f(irisParameterTangentPoint) = point where line touches irisEllipse.
    //i.e. find the gradient from tangentAngle.  Find the derivative of ellipses.  equate and solve for the parameter.
    G4double irisParameterTangentPoint = atan(-irisRSemiAxis/(m*irisZSemiAxis)) + CLHEP::pi; //Add pi to get desired solution (third quadrant)
    G4double equatorParameterTangentPoint = atan(-equatorRSemiAxis/(m*equatorZSemiAxis));  //first quadrant as desired.
    std::cout << "irisParameterTangentPoint = " << irisParameterTangentPoint << std::endl;
    std::cout << "equatorParameterTangentPoint = " << equatorParameterTangentPoint << std::endl;
    //Using values of z and y that give the cartesian coords of the tangent points (from the parameters)
    //iris ellipse tangent point:
    //  G4double irisZIntercept = zi + irisZSemiAxis*cos(irisParameterTangentPoint); 
    //G4double irisRIntercept = ri + irisRSemiAxis*sin(irisParameterTangentPoint);
    //equator ellipse tangent point:
    //    G4double equatorZIntercept = ze + equatorZSemiAxis*cos(equatorParameterTangentPoint);
    //G4double equatorRIntercept = re + equatorRSemiAxis*sin(equatorParameterTangentPoint);

    
    
    //The equator and iris ellipses are defined parametrically, equator(iris)Parameter is the parameter for generating the equator (iris) ellipse.

    //least ambiguous distribution of noPoints: quarter in iris ellipse, half in dome
    //and another quarter in the iris on other side
    //therefore noPoints ought to be a multiple of four:
   
    noPoints = noPoints - (noPoints % 4);
    //little warning message here maybe that noPoints is being rounded..
    G4int quarterNoPoints = noPoints / 4 ;            //each quarter is made up of quarterNoPoints
    std::vector<G4double> equatorParameter;
    std::vector<G4double> irisParameter;
    std::vector<G4double> zCoord;
    std::vector<G4double> rInnerCoord;
    std::vector<G4double> rOuterCoord;

    
    //Generating the points for half cavity. I can then reflect in R-axis for full curve

    std::cout << "noPoints = " << noPoints << std::endl;
    
    std::cout << "ri = " << ri << std::endl;
    std::cout << "zi = " << zi << std::endl;
    std::cout << "re = " << re << std::endl;
    std::cout << "ze = " << ze << std::endl;

    std::cout << "equatorRadius = " << equatorRadius << std::endl;
    std::cout << "equatorRSemiAxis = " << equatorRSemiAxis << std::endl;

    //populating values for the parameters. Starting from -z through to +z.
    for (int i = 0; i < noPoints/4; i++) //I need to edit this so that equatorParameter runs for noPoints/2.  This will ensure the total number of points is indeed noPoints
      {
	irisParameter.push_back(1.5*CLHEP::pi + (1.5*CLHEP::pi - irisParameterTangentPoint )*i/((noPoints/4)-1));  //filling aray with points between bottom (3pi/2) and iris tangent point
	std::cout << "irisParameter[" << i << "] =" << irisParameter[i] << std::endl;
      };

    //populating parameter values for the entire equator ellipse from the tangent point on one side to the tangent point on the other side.
    for (int i = 0; i < noPoints/2; i++)
      {
	equatorParameter.push_back((CLHEP::pi - equatorParameterTangentPoint) - (CLHEP::pi - 2* equatorParameterTangentPoint)*i/((noPoints/2)-1)); //filling array with points between equator tangentpoint and pi/2
	std::cout << "equatorParameter[" << i << "] =" << equatorParameter[i] << std::endl;	
      };

    std::cout << "ze = " << ze << std::endl;    
    std::cout << "zi = " << zi << std::endl;
    std::cout << "re = " << re << std::endl;
    std::cout << "ri = " << ri << std::endl;

    std::cout << "left iris points:" << std::endl;
    for (int i = 0; i < noPoints/4; i++)
      {
	zCoord.push_back(- zi + irisZSemiAxis * cos(irisParameter[i]));
	rInnerCoord.push_back(ri + irisRSemiAxis * sin(irisParameter[i]));
	std::cout << "zCoord["  << i << "]" << " = " << zCoord[i] << std::endl ;
      };
    std::cout << "entire equator beginning from -z through to +z " << std::endl;



    for (int i = 0; i < noPoints/2; i++)
      {
	zCoord.push_back(ze + equatorZSemiAxis * cos(equatorParameter[i]));
	rInnerCoord.push_back(re + equatorRSemiAxis * sin(equatorParameter[i]));
	std::cout << "zCoord["  << i << "]" << " = " << zCoord[noPoints/4+i] << std::endl ;
      }

    std::cout << "right iris points" << std::endl;
    for (int i = 0; i < noPoints/4; i++)
      
      {
	zCoord.push_back(zi + irisZSemiAxis * cos(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));
	rInnerCoord.push_back(ri + irisRSemiAxis * sin(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));
	std::cout << "zCoord["  << i << "]" << " = " << zCoord[3*noPoints/4 + i] << std::endl ;
      };
    

    
    double gradientAt_i = 0; //double to store the gradient at each point
    std::cout << "printing (z,r):" << std::endl;
    for(int i = 0; i < zCoord.size(); ++i)
      {
	std::cout << "element["  << i << "]" << " = " << "(" << zCoord[i] << "," << rInnerCoord[i] << ")" << std::endl;
      };
    
    std::cout << "numer of zcoords = " << zCoord.size() << std::endl;
    std::cout << "numer of rInnerCoords = " << rInnerCoord.size() << std::endl;

    
    for (int i = 0; i <  noPoints; i++)
      {
	if (i == 0 || i == (noPoints - 1))
	  {
	    rOuterCoord.push_back(rInnerCoord[i] + thickness) ;
	  }
	else
	  {
	    G4double result;
	    //finding outer point perpendicular from inner point:
	    gradientAt_i = (rInnerCoord[i-1]-rInnerCoord[i+1])/(zCoord[i-1]-zCoord[i+1]) ;
	    // zCoord[i] = -gradientAt_i * thickness * pow(gradientAt_i * gradientAt_i + 1, -0.5) + zCoord[i];
	    result = (thickness * pow( (gradientAt_i * gradientAt_i + 1),-0.5) + rInnerCoord[i]);
	    //Reassign rOuterPoint from the perpendicular point to the vertical point
	    rOuterCoord.push_back(rInnerCoord[i] + pow(thickness, 2.0)/(result - rInnerCoord[i]));
      	      
	  }
      };
    
    std::cout << "building cavity solid " << std::endl;
    std::cout << "number of points= " << noPoints << std::endl;
    std::cout << "zrood=  " << zCoord.size() << std::endl;
    std::cout << "rinner = " <<rInnerCoord.size() << std::endl;
    std::cout << "r outer = "<<rOuterCoord.size() << std::endl;
    cavitySolid = new G4Polycone(name + "_cavity_solid", //name
				 0.0,                     //start angle  
				 2*CLHEP::pi,             //sweep angle
				 noPoints,        //number of planes
				 zCoord.data(),                //zaxis points
				 rInnerCoord.data(),                //inner radial points
				 rOuterCoord.data()
				 );

    cavityLV = new G4LogicalVolume(cavitySolid,            //solid
				   cavityMaterial,         //material
				   name + "_cavity_lv"     //name
				   );                      

    G4VisAttributes* cavityVis = new G4VisAttributes();

    cavityVis->SetColour(0.69,0.769,0.871); //light steel blue
    cavityVis->SetVisibility(true);          //visible
    cavityLV->SetVisAttributes(cavityVis);   //give  colour+visibility to the cavity logical volume

    //Vaccuum:
    std::cout << "building vacuum:" << std::endl;
    //Making radial distance smaller by lengthSafety to ensure no overlap between vacuum and cavity solid, also ends will over lap so first and last z values are also reduced
    for (int i = 0; i < noPoints; i++)
      {
	rInnerCoord[i] = rInnerCoord[i] - lengthSafety;
      };
    zCoord[0] = zCoord[0] - lengthSafety;
    zCoord[noPoints - 1] = zCoord[noPoints - 1] - lengthSafety;

    std::cout << "building vacuum solid" << std::endl;
    vacuumSolid = new G4Polycone(name + "_vacuum_solid", //name
				 0.0,                     //start angle  
				 2*CLHEP::pi,             //sweep angle
				 noPoints,        //number of planes
				 rInnerCoord.data(),     //radial coordinate of corner
				 zCoord.data()      //axial coordinate of corner.
				 );

    vacuumLV = new G4LogicalVolume(vacuumSolid,           //solid
				  vacuumMaterial,              //material
				  name + "_vacuum_lv");

    G4VisAttributes* vacuumVis = new G4VisAttributes(); //vistattributes instance 
    vacuumVis->SetVisibility(false);                    //Make invisible
    vacuumLV->SetVisAttributes(vacuumVis);              //give invisiblity to the vacuum LV.

    
   
    

  
        
  };

void BDSCavity::BuildPillBoxCavityGeometry()
{

  //irisRadius should perhaps be changed to some variable higher up the class
  //structure using beamPipeInfo?  
  G4VSolid* outerSolid = new G4Tubs(name + "_outer_solid", //name
				    irisRadius,                   //inner radius
				    equatorRadius - lengthSafety, //equatorRadius
				    0.5 * chordLength - lengthSafety, //half length
				    0.0,                           //startAngle
				    2*CLHEP::pi                    //spanningAngle
				    );
  
  
  //Creates a cylinder from which to subtract from larger cylinder.
  //i.e hollows out the cavity
  innerSolid = new G4Tubs(name + "_inner_solid", //name
			  0.0,            //inner radius 
			  equatorRadius - thickness,           //equatorRadius
			  0.5 * (chordLength - thickness),     //Half length
			  0.0,                   //starAngle
			  2*CLHEP::pi           //spanningAngle
			  );
  
  cavitySolid = new G4SubtractionSolid(name + "_collimator_solid",  //name
				       outerSolid,                //solid1
				       innerSolid                 //minus solid2
				       );

  //Vacuum:  Union of two solids.  One cylinder (VacuumInnerCavity) to fill the centre, and a longer,
  // thinner cylinder (vaccumAperture) to fill the irises -> vacuumSolid
  
  G4VSolid* vacuumInnerCavity = new G4Tubs(name + "_vacuum_inner_cavity_solid",                       //name
					   0.0,                                                //inner radius
					   (equatorRadius - thickness) - lengthSafety,           //outer radius
					   0.5 * (chordLength - thickness) - lengthSafety,     //half length
					   0.0,                                                //start angle
					   2*CLHEP::pi                                        //spanning angle
					   );
  
  G4VSolid* vacuumAperture = new G4Tubs(name +"_vacuum_aperture_solid",        //name
					0.0,                             //inner radius
					irisRadius - lengthSafety,       //outer radius
					0.5 * chordLength - lengthSafety,//length
					0.0,                             //start angle
					2*CLHEP::pi                     //spanning angle
					);
  vacuumSolid = new G4UnionSolid(name + "_vacuum_solid",  //name
				 vacuumInnerCavity,       //solid one
				 vacuumAperture           //Added to solid two.
				 );
  
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
						      cavityMaterial,          // material
						      name + "_cavity_lv"); // name

  G4VisAttributes* cavityVis = new G4VisAttributes();

  cavityVis->SetColour(0.69,0.769,0.871); //light steel blue
  cavityVis->SetVisibility(true);          //visible
  cavityLV->SetVisAttributes(cavityVis);   //give  colour+visibility to the cavity logical volume

  
  vacuumLV = new G4LogicalVolume(vacuumSolid,           //solid
				 vacuumMaterial,              //material
				 name + "_vacuum_lv");

  G4VisAttributes* vacuumVis = new G4VisAttributes(); //vistattributes instance 
  vacuumVis->SetVisibility(false);                    //Make invisible
  vacuumLV->SetVisAttributes(vacuumVis);              //give invisiblity to the vacuum LV.

  
  //There is thickness*radius*radius*pi voulme in either end which has no vacuum..  use boolean ops (addition) to build
  
}
